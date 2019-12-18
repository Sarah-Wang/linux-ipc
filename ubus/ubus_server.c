#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <semaphore.h>
#include <pthread.h>
#include <libubus.h>
#include <libubox/blobmsg_json.h>

static int nn_cam_handler(struct ubus_context *ctx,
				struct ubus_object *obj,
				struct ubus_request_data *req,
				const char *method,
				struct blob_attr *msg);


static const struct blobmsg_policy nn_cam_policy[] = {
	[0] = {.name = "count", .type = BLOBMSG_TYPE_INT32},
	[1] = {.name = "timeout", .type = BLOBMSG_TYPE_INT32},
};

static int nn_run_handler(struct ubus_context *ctx,
				struct ubus_object *obj,
				struct ubus_request_data *req,
				const char *method,
				struct blob_attr *msg);


static const struct blobmsg_policy nn_run_policy[] = {
	[0] = {.name = "userid", .type = BLOBMSG_TYPE_STRING},
	[1] = {.name = "image_path_list",
		.type = BLOBMSG_TYPE_ARRAY},
};

static const struct ubus_method nn_methods[] = {
	UBUS_METHOD("jpeg_register", nn_run_handler, nn_run_policy),
	UBUS_METHOD("camera_register", nn_cam_handler, nn_cam_policy),
};

static struct ubus_object_type nn_obj_type =
	UBUS_OBJECT_TYPE("nn_obj", nn_methods);

static struct ubus_object nn_obj = {
	.name = "fr",
	.type = &nn_obj_type,
	.methods = nn_methods,
	.n_methods = ARRAY_SIZE(nn_methods),
};

static int nn_cam_handler(struct ubus_context *ctx,
				struct ubus_object *obj,
				struct ubus_request_data *req,
				const char *method,
				struct blob_attr *msg)
{
	int timeout = 0;
	int count = 0;
	struct blob_attr *tb[2];
	struct blob_buf bb = {0};
	void *tbl = NULL;
	char *val = "this is a fake value";

	printf("nn_cam_handler\n");
	blobmsg_parse(nn_cam_policy,
		ARRAY_SIZE(nn_cam_policy),
		tb,
		blob_data(msg),
		blob_len(msg));

	if (tb[0]) {
		count = blobmsg_get_u32(tb[0]);
		printf("count: %d\n", count);
	}

	if (tb[1]) {
		timeout = blobmsg_get_u32(tb[1]);
		printf("timeout: %d\n", timeout);
	}

	blob_buf_init(&bb, 0);
	blobmsg_add_u32(&bb, "ret", 0);
	blobmsg_add_string(&bb, "errstring", "ok");
	tbl = blobmsg_open_array(&bb, "value");
	blobmsg_add_string(&bb, NULL, val);
	blobmsg_add_string(&bb, NULL, val);
	blobmsg_close_table(&bb, tbl);
	ubus_send_reply(ctx, req, bb.head);
	return 0;
}

static int nn_run_handler(struct ubus_context *ctx,
				struct ubus_object *obj,
				struct ubus_request_data *req,
				const char *method,
				struct blob_attr *msg)
{
	int ret = 0;
	char *userid = NULL;
	char path[256] = {0};
	struct blob_attr *tb[2];
	struct blob_buf bb = {0};
	void *tbl = NULL;
	char *val = "this is a fake value";

	printf("nn_run_handler\n");
	blobmsg_parse(nn_run_policy,
		ARRAY_SIZE(nn_run_policy),
		tb,
		blob_data(msg),
		blob_len(msg));

	if (tb[0]) {
		userid = blobmsg_get_string(tb[0]);
		printf("userid: %s\n", userid);
	}

	if (tb[1]) {
		struct blob_attr *head = blobmsg_data(tb[1]);
		struct blob_attr *attr;
		int len = blobmsg_data_len(tb[1]);

		__blob_for_each_attr(attr, head, len) {
			if (blob_id(attr) == BLOBMSG_TYPE_STRING) {
				printf("data: %s\n",
						(char *)(blobmsg_data(attr)));
				strcpy(path, (char *)(blobmsg_data(attr)));
			}
		}
	}

	blob_buf_init(&bb, 0);
	blobmsg_add_string(&bb, "userid", userid);
	tbl = blobmsg_open_array(&bb, "path_list");
	blobmsg_add_string(&bb, NULL, path);
	blobmsg_close_table(&bb, tbl);
	tbl = blobmsg_open_array(&bb, "value_list");
	blobmsg_add_string(&bb, NULL, val);
	blobmsg_close_table(&bb, tbl);
	ubus_send_reply(ctx, req, bb.head);
	return 0;
}

int main(void)
{
	int ret = 0;
	const char *ubus_socket = NULL;
	struct ubus_context *ubus_ctx;

	uloop_init();

	ubus_ctx = ubus_connect(ubus_socket);
	if (ubus_ctx == NULL) {
		printf("Fail to connect to ubusd!\n");
		return 0;
	}

	ubus_add_uloop(ubus_ctx);
	ret = ubus_add_object(ubus_ctx, &nn_obj);
	if (ret) {
		printf("Fail to register an object to ubus.\n");
		goto UBUS_FAIL;
	} else {
		printf("Add '%s' to ubus @%u successfully.\n",
			nn_obj.name, nn_obj.id);
	}

	uloop_run();

UBUS_FAIL:
	uloop_done();
	ubus_free(ubus_ctx);
	return ret;
}

