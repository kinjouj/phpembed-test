#include <client.hh>

using namespace std;

long invoke_method(zval* val, const char* method,zval** retval) {
    if (Z_TYPE_P(val) != IS_OBJECT) {
        return -1;
    }

    zval fname;
    ZVAL_STRING(&fname, method, 0);

    return call_user_function_ex(NULL, &val, &fname, retval, 0, NULL, 0, NULL TSRMLS_CC);
}

void execute_closure(zval* val) {
    zval fname;
    ZVAL_STRING(&fname, "__invoke", 0);

    zval* retval;

    long status;
    status = invoke_method(val, (const char*)"__invoke", &retval);

    if (status == SUCCESS) {
        int type;
        type = Z_TYPE_P(retval);

        switch(type) {
            case IS_STRING:
                cout << "[OBJECT][Closure]" << Z_STRVAL_P(retval) << endl;

                break;

            case IS_LONG:
                cout << "[OBJECT][Closure]" << Z_LVAL_P(retval) << endl;

                break;

            case IS_ARRAY:
                recursive_array(retval);

                break;
        }
    }
}

void recursive_array(zval* val) {
    long size = 0;

    if (Z_TYPE_P(val) == IS_ARRAY) {
        size = zend_hash_num_elements(Z_ARRVAL_P(val));
        //size = retval->value.ht->nNumOfElements;
    }

    int i;

    for (int i = 0;i < size;i++) {
        zval** data;
        zend_hash_get_current_data(val->value.ht, (void**)&data);

        int type;
        type = Z_TYPE_PP(data);

        cout << "[ARRAY][Type] " << type << endl;

        switch(type) {
            case IS_STRING:
                cout << "[ARRAY][Value] " << Z_STRVAL_PP(data) << endl;

                break;
            case IS_LONG:
                cout << "[ARRAY][Value] " << Z_LVAL_PP(data) << endl;

                break;

            case IS_ARRAY:
                recursive_array(*data);

                break;

            case IS_OBJECT:
                zend_class_entry* ce;
                ce = Z_OBJCE_PP(data);

                if (strcmp(ce->name, (char*)"Closure") == 0) {
                    execute_closure(*data);
                }
        }

        zend_hash_move_forward(val->value.ht);
    }
}

vector<string> buffers;

int ub_write_func(const char* buf, unsigned int n) {
    buffers.push_back(string(buf));

    return 0;
}

int header_handler(sapi_header_struct * sapiHeader, sapi_header_op_enum op, sapi_headers_struct* sapi_headers TSRMLS_DC) {
    cout << "header: " << sapiHeader->header << endl;
}

int main(int argc, char** argv) {
    php_embed_module.ub_write = ub_write_func;
    php_embed_module.header_handler = header_handler;

    PHP_EMBED_START_BLOCK(argc, argv);

    zend_file_handle script;
    script.type = ZEND_HANDLE_FILENAME;
    script.filename = (char*)"test.php";
    script.opened_path = NULL;
    script.free_filename = 0;

    zend_try {
        zval* retval;
        MAKE_STD_ZVAL(retval);

        zend_execute_scripts(ZEND_REQUIRE TSRMLS_CC, &retval, 1, &script);

        int size;
        size = buffers.size();

        if (size > 0) {
            int i;

            for (vector<string>::const_iterator itr = buffers.begin(); itr != buffers.end(); ++itr) {
                string t = *itr;

                cout << t << endl;
            }
        }

        if (Z_TYPE_P(retval) == IS_STRING) {
            cout << "[STRING] " << retval->value.str.val << endl;
        } else if(Z_TYPE_P(retval) == IS_ARRAY) {
            recursive_array(retval);
        } else if (Z_TYPE_P(retval) == IS_OBJECT) {
            zend_class_entry* ce;
            ce = Z_OBJCE_P(retval); // ce = zend_get_class_entry(retval);

            cout << "[OBJECT][Class][" << ce->name << "]" << endl;

            zval* retval2;

            if (strcmp(ce->name, (char*)"Sample") == 0) {
                char* propname;
                propname = (char*)"message";

                zval* property;
                property = zend_read_property(ce, retval, propname, strlen(propname), 0);

                cout << "[OBJECT][Class][" << ce->name << "][Property:message] " << Z_STRVAL_P(property) << endl;

                zval fname;
                ZVAL_STRING(&fname, "getMessage", 0);

                int status;
                status = invoke_method(retval, "getMessage", &retval2);

                if (status == SUCCESS) {
                    if (Z_TYPE_P(retval2) == IS_STRING) {
                        cout << "[OBJECT][Class][" << ce->name << "][Method:getMessage]: " << Z_STRVAL_P(retval2) << endl;
                    }
                }
            } else if(strcmp(ce->name, (char*)"Closure") == 0) {
                execute_closure(retval);
            }
        }
    } zend_end_try();

    PHP_EMBED_END_BLOCK();

    cout << "buffer size: " << buffers.size() << endl;

    return 0;
}
