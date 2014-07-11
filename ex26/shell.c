#include "shell.h"
#include "dbg.h"

int Shell_exec(Shell template, const char *opts[]) {
    apr_pool_t *p = NULL;
    apr_status_t rv = APR_SUCCESS;
    int i = 0;
    int j = 0;
    int rc = -1;
    int count = 0;

    rv = apr_pool_create(&p, NULL);
    check(rv == APR_SUCCESS, "Failed to create pool.");

    for (i = 0; opts[i] != NULL; i++) {
        for (j = 0; template.args[j] != NULL; j++) {
            if (strcmp(template.args[j], opts[i]) == 0) {
                template.args[j] = opts[++i];
                count++;
                break;
            }
        }
    }
    check(template.vargs == count, "Invalid number of arguments.");

    rc = Shell_run(p, &template);
    apr_pool_destroy(p);
    return rc;

error:
    if (p) apr_pool_destroy(p);
    return rc;
}

int Shell_run(apr_pool_t *p, Shell *cmd) {
    apr_procattr_t *attr;
    apr_status_t rv;
    apr_proc_t newproc;

    rv = apr_procattr_create(&attr, p);
    check(rv == APR_SUCCESS, "Failed to create proc attr.");

    rv = apr_procattr_io_set(attr, APR_NO_PIPE, APR_NO_PIPE, APR_NO_PIPE);
    check(rv == APR_SUCCESS, "Failed to set IO of command.");

    rv = apr_procattr_dir_set(attr, cmd->dir);
    check(rv == APR_SUCCESS, "Failed to set root to %s", cmd->dir);

    rv = apr_procattr_cmdtype_set(attr, APR_PROGRAM_PATH);
    check(rv == APR_SUCCESS, "Failed to set cmd type.");

    rv = apr_proc_create(&newproc, cmd->exe, cmd->args, NULL, attr, p);
    check(rv == APR_SUCCESS, "Failed to run command.");

    rv = apr_proc_wait(&newproc, &cmd->exit_code, &cmd->exit_why, APR_WAIT);
    check(rv == APR_CHILD_DONE, "Failed to wait.");

    printf("exit code is %d\n", cmd->exit_code);
    check(cmd->exit_code == 0, "%s exited badly.", cmd->exe);
    check(cmd->exit_why == APR_PROC_EXIT, "%s was killed or crashed", cmd->exe);

    return 0;

error:
    return -1;
}

Shell CLEANUP_SH = {
    .exe = "rm",
    .dir = "/tmp",
    .args = {"rm", "-rf", "/tmp/pkg-build", "/tmp/pkg-src.tar.gz",
        "/tmp/pkg-src.tar.bz2", "/tmp/DEPENDS", NULL},
    .vargs = 0
};

Shell GIT_SH = {
    .dir = "/tmp",
    .exe = "git",
    .args = {"git", "clone", "URL", "pkg-build", NULL},
    .vargs = 1
};

Shell TAR_SH = {
    .dir = "/tmp/pkg-build",
    .exe = "tar",
    .args = {"tar", "-xzf", "FILE", "--strip-components", "1", NULL},
    .vargs = 1
};

Shell CURL_SH = {
    .dir = "/tmp",
    .exe = "curl",
    .args = {"curl", "-L", "-o", "TARGET", "URL", NULL},
    .vargs = 2
};

Shell CONFIGURE_SH = {
    .exe = "./configure",
    .dir = "/tmp/pkg-build",
    .args = {"configure", "OPTS", NULL},
    .vargs = 1
};

Shell MAKE_SH = {
    .exe = "make",
    .dir = "/tmp/pkg-build",
    .args = {"make", "OPTS", NULL},
    .vargs = 1
};

Shell INSTALL_SH = {
    .exe = "make",
    .dir = "/tmp/pkg-build",
    .args = {"make", "TARGET", NULL},
    .vargs = 1
};

