#include <server.hh>

using namespace std;

struct evbuffer* buffer;

int ub_write_handler(const char* c, unsigned int n) {
    evbuffer_add(buffer, c, strlen(c));

    return -1;
}

static void process_request(struct evhttp_request* req, void* argv) {
    zend_try {
        zend_file_handle script;
        script.type = ZEND_HANDLE_FILENAME;
        script.filename = (char*)"test.php";
        script.opened_path = NULL;
        script.free_filename = 1;

        buffer = evbuffer_new();

        zval* retval;

        zend_execute_scripts(ZEND_REQUIRE TSRMLS_CC, &retval, 1, &script);

        evhttp_add_header(evhttp_request_get_output_headers(req), "Content-Type", "text/plain");

        evhttp_send_reply(req, HTTP_OK, "OK", buffer);

        evbuffer_free(buffer);

        // evhttp_send_reply(req, HTTP_NOTFOUND, "Not Found", NULL);

        /*
        if (Z_TYPE_P(retval) == IS_STRING) {
            struct evbuffer *buf;
            buf = evbuffer_new();

            char* text = Z_STRVAL_P(retval);

            evbuffer_add(buf, text, strlen(text));
            evhttp_send_reply(req, HTTP_OK, "OK", buf);
            evbuffer_free(buf);
        } else {
            evhttp_send_reply(req, HTTP_NOTFOUND, "Not Found", NULL);
        }
        */
    } zend_end_try();
}

int main(int args,char** argv) {
    struct event_base* base;
    base = event_init();

    if (base == NULL) {
        cerr << "cannot create event_base_new" << endl;

        return -1;
    }

    struct evhttp* httpd;
    httpd = evhttp_start("127.0.0.1", 8080);

    if (httpd == NULL) {
        cerr << "cannot create evhttp_new" << endl;

        return -1;
    }

    evhttp_set_gencb(httpd, process_request, NULL);

    php_embed_module.ub_write = ub_write_handler;

    PHP_EMBED_START_BLOCK(args, argv);

    event_dispatch();

    PHP_EMBED_END_BLOCK();

    evhttp_free(httpd);
    event_base_free(base);

    return 0;
}
