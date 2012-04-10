#include <client.hh>

using namespace std;

void invoke_method(zval* val, const char* method) {

}

void execute_closure(zval* val) {
    zval fname;
    ZVAL_STRING(&fname, "__invoke", 0);

    zval* retval;

    if (call_user_function_ex(NULL, &val, &fname, &retval, 0, NULL, 0, NULL TSRMLS_CC) == SUCCESS) {
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
    size = zend_hash_num_elements(Z_ARRVAL_P(val)); //size = retval->value.ht->nNumOfElements;

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

int main(int argc, char** argv) {
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

                if (call_user_function_ex(NULL, &retval, &fname, &retval2, 0, NULL, 0,NULL TSRMLS_CC) == SUCCESS) {
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

    return 0;
}
