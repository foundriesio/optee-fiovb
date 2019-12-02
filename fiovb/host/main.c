/*
 * Copyright (c) 2019, Foundries.IO
 */

#include <err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <tee_client_api.h>
#include <ta_fiovb.h>

#define	CMD_PRINTENV	"fiovb_printenv"
#define CMD_SETENV	"fiovb_setenv"
#define MAX_BUFFER	4096

struct fiovb_ctx {
	TEEC_Context ctx;
	TEEC_Session sess;
};

void prepare_tee_session(struct fiovb_ctx *ctx)
{
	TEEC_UUID uuid = TA_FIOVB_UUID;
	uint32_t origin;
	TEEC_Result res;

	res = TEEC_InitializeContext(NULL, &ctx->ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

	res = TEEC_OpenSession(&ctx->ctx, &ctx->sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
			res, origin);
}

void terminate_tee_session(struct fiovb_ctx *ctx)
{
	TEEC_CloseSession(&ctx->sess);
	TEEC_FinalizeContext(&ctx->ctx);
}

static int read_persistent_value(const char *s)
{
	char req[MAX_BUFFER] = { '\0' };
	char rsp[MAX_BUFFER] = { '\0' };
	struct fiovb_ctx ctx;
	TEEC_Operation op;
	TEEC_Result res;
	uint32_t origin;

	prepare_tee_session(&ctx);

	strcpy(req, s);
	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
					 TEEC_MEMREF_TEMP_INOUT,
					 TEEC_NONE, TEEC_NONE);
	op.params[0].tmpref.buffer = req;
	op.params[0].tmpref.size = strlen(req) + 1;
	op.params[1].tmpref.buffer = rsp;
	op.params[1].tmpref.size = MAX_BUFFER - 1;

	res = TEEC_InvokeCommand(&ctx.sess, TA_FIOVB_CMD_READ_PERSIST_VALUE,
				 &op, &origin);

	terminate_tee_session(&ctx);

	if (res == TEEC_SUCCESS) {
		fprintf(stderr, "%s\n", rsp);
		return 0;
	}
		
	fprintf(stderr, "READ_PVALUE failed: 0x%x / %u\n", res, origin);

	return -1;
}

static int write_persistent_value(const char *name, const char *value)
{
	char req1[MAX_BUFFER] = { '\0' };
	char req2[MAX_BUFFER] = { '\0' };

	struct fiovb_ctx ctx;
	TEEC_Operation op;
	TEEC_Result res;
	uint32_t origin;

	prepare_tee_session(&ctx);

	strcpy(req1, name);
	strcpy(req2, value);
	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
					 TEEC_MEMREF_TEMP_INPUT,
					 TEEC_NONE, TEEC_NONE);
	op.params[0].tmpref.buffer = req1;
	op.params[0].tmpref.size = strlen(req1) + 1;
	op.params[1].tmpref.buffer = req2;
	op.params[1].tmpref.size = strlen(req2) + 1;

	res = TEEC_InvokeCommand(&ctx.sess, TA_FIOVB_CMD_WRITE_PERSIST_VALUE,
				 &op, &origin);

	terminate_tee_session(&ctx);

	if (res == TEEC_SUCCESS)
		return 0;

	fprintf(stderr, "WRITE_PVALUE failed: 0x%x / %u\n", res, origin);

	return -1;
}

static int fiovb_printenv(int argc, char *argv[])
{
	if (argc != 2)
		return -1;

	return read_persistent_value(argv[1]);
}

int fiovb_setenv(int argc, char *argv[])
{
	if (argc != 3)
		return -1;

	return write_persistent_value(argv[1], argv[2]);
}

int main(int argc, char *argv[])
{
	char *p, *cmdname = *argv;

	if ((p = strrchr (cmdname, '/')) != NULL)
		cmdname = p + 1;

	if (!strncmp(cmdname, CMD_PRINTENV, strlen(CMD_PRINTENV))) {
		if (fiovb_printenv (argc, argv)) {
			fprintf (stderr, "Cant print the environment\n");
			return EXIT_FAILURE;
		}

		return EXIT_SUCCESS;
	}

	if (!strncmp(cmdname, CMD_SETENV, strlen(CMD_SETENV))) {
		if (fiovb_setenv(argc, argv)) {
			fprintf (stderr, "Cant set the environment\n");
			return EXIT_FAILURE;
		}

		return EXIT_SUCCESS;
	}

	fprintf (stderr,
		"Identity crisis - may be called as `" CMD_PRINTENV
		"' or as `" CMD_SETENV "' but not as `%s'\n", argv[0]);

	return EXIT_FAILURE;
}

