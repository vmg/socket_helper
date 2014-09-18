#include <ruby.h>
#include <ruby/io.h>
#include <sys/socket.h>

extern VALUE rb_thread_io_blocking_region(rb_blocking_function_t *func, void *data1, int fd);
#define BLOCKING_REGION_FD(func, arg) (long)rb_thread_io_blocking_region((func), (arg), (arg)->fd)

struct recv_args {
	VALUE buffer;
	int fd, flags;
	long pos;
};

static VALUE
recv_loop(void *data)
{
    struct recv_args *arg = data;
    ssize_t ret;

    ret = recv(arg->fd,
		RSTRING_PTR(arg->buffer) + arg->pos,
		RSTRING_LEN(arg->buffer) - arg->pos,
		arg->flags
	);

	if (ret < 0)
		return (VALUE)ret;

	arg->pos += ret;
    return (VALUE)ret;
}

static VALUE rb_bertrpc_socket_read_in_full(int argc, VALUE *argv, VALUE self)
{
	rb_io_t *fptr;
	VALUE rb_sock, rb_buffer, rb_klass;
	struct recv_args arg;
	long buflen;

    VALUE len, flg;
    rb_scan_args(argc, argv, "21", &rb_sock, &len, &flg);

    if (flg == Qnil)
		arg.flags = 0;
    else
		arg.flags = NUM2INT(flg);

    buflen = NUM2INT(len);

	GetOpenFile(rb_sock, fptr);
	if (rb_io_read_pending(fptr)) {
		rb_raise(rb_eIOError, "recv for buffered IO");
	}

	arg.fd = fptr->fd;
	arg.pos = 0;
	arg.buffer = rb_buffer = rb_tainted_str_new(0, buflen);

    rb_klass = RBASIC(rb_buffer)->klass;
	rb_obj_hide(rb_buffer);

	while (arg.pos < buflen) {
		long slen;

		rb_io_check_closed(fptr);
		slen = BLOCKING_REGION_FD(recv_loop, &arg);

		if (!slen)
			break;

		if (!rb_io_wait_readable(fptr->fd))
			rb_sys_fail("recvfrom(2)");
	}

	rb_obj_reveal(rb_buffer, rb_klass);
	if (arg.pos < buflen)
		rb_raise(rb_eRuntimeError, "connection closed");

	rb_obj_taint(rb_buffer);
	return rb_buffer;
}

void Init_socket_helper(void)
{
	VALUE rb_mSocketHelper = rb_define_module("SocketHelper");
	rb_define_module_function(rb_mSocketHelper, "read_in_full", rb_socket_read_in_full, -1);
}
