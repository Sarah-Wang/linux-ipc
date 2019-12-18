#include <stdio.h>
#include <libubus.h>
#include <libubox/blobmsg_json.h>


static struct ubus_context *ctx;
static struct blob_buf bb;

static void resp_handler(struct ubus_request *req, int type,
		struct blob_attr *msg)
{
	char *strmsg;

	if (!msg)
		return;

	/* 0 type of format */
	strmsg = blobmsg_format_json_indent(msg, true, 0);
	printf("\n\nResponse from the host: %s\n", strmsg);
	 /* need to free strmsg */
	free(strmsg);
}

struct nn_args {
	char userid[129];
	char oper_id[129];
	int face_num;
	char path[12][256];
};

static void resp_handler_args(struct ubus_request *req, int type,
		struct blob_attr *msg)
{
	char *strmsg;
	struct nn_args *arg = (struct nn_args *)req->priv;

	if (!msg)
		return;

	/* 0 type of format */
	strmsg = blobmsg_format_json_indent(msg, true, 0);
	printf("\n\nResponse from the host: %s\n", strmsg);
	 /* need to free strmsg */
	free(strmsg);

	if (arg) {
		printf("%s\n", arg->userid);
		printf("%s\n", arg->oper_id);
	} else {
		printf("arg is null\n");
	}
}


void main(void)
{
	int ret = 0;
	uint32_t adp_id;
	const char *ubus_socket = NULL;
	struct nn_args args = {
		"sarah",
		"1234",
		4,
		{"1", "2"},
	};

	/* 1. create an epoll instatnce descriptor poll_fd */
	uloop_init();

	/* 2. connect to ubusd and get ctx */
	ctx = ubus_connect(ubus_socket);
	if (!ctx) {
		printf("Fail to connect to ubusd!\n");
		return;
	}

	/* 3. registger epoll events to uloop, start sock listening */
	ubus_add_uloop(ctx);


	/* 4. search a registered object with a given name */
	if (ubus_lookup_id(ctx, "adapter", &adp_id)) {
		printf("adapter_run is NOT found in ubus!\n");
		goto UBUS_FAIL;
	}
	printf("adapter server is found in ubus @%u.\n", adp_id);


	/* 5. call a ubus method */
	blob_buf_init(&bb, 0);
	blobmsg_add_string(&bb, "cmd", "get_admin_info");
	/* blobmsg_add_field(&bb, BLOBMSG_TYPE_STRING, "data", */
			/* &d, sizeof(d)); */
	ret = ubus_invoke(ctx, adp_id, "adapter_run", bb.head,
			resp_handler_args, &args, 3000000);
	printf("adapter run get admin info result: %s\n",
			ubus_strerror(ret));


	/* 5. call a ubus method */
	blob_buf_init(&bb, 0);
	blobmsg_add_string(&bb, "cmd", "get_perm_version");
	blobmsg_add_string(&bb, "data", "");
	ret = ubus_invoke(ctx, adp_id, "adapter_run", bb.head,
			resp_handler, 0, 3000000);
	printf("adapter run get perm version result: %s\n",
			ubus_strerror(ret));


	/* 5. call a ubus method */
	blob_buf_init(&bb, 0);
	blobmsg_add_string(&bb, "cmd", "ping");
	ret = ubus_invoke(ctx, adp_id, "adapter_run", bb.head,
			resp_handler, 0, 3000000);
	printf("adapter run ping result: %s\n",
			ubus_strerror(ret));

	/* 5. call a ubus method */
	blob_buf_init(&bb, 0);
	blobmsg_add_string(&bb, "cmd", "upload_userinfo");
	blobmsg_add_string(&bb, "data", "");
	ret = ubus_invoke(ctx, adp_id, "adapter_run", bb.head,
			resp_handler, 0, 3000000);
	printf("adapter run ping result: %s\n",
			ubus_strerror(ret));


	/* 6. end ubus */
	uloop_done();
UBUS_FAIL:
	ubus_free(ctx);
}



