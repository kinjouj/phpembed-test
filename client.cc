#include <iostream>
#include <sapi/embed/php_embed.h>

using namespace std;

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
            long size = 0;
            size = zend_hash_num_elements(Z_ARRVAL_P(retval));
            //size = retval->value.ht->nNumOfElements;

            cout << "[ARRAY]size: " << size << endl;

            int i;

            for (i = 0;i < size;i++) {
                zval** data;
                zend_hash_get_current_data(retval->value.ht, (void**)&data);

                if (Z_TYPE_PP(data) == IS_STRING) {
                    cout << Z_STRVAL_PP(data) << endl;
                }

                zend_hash_move_forward(retval->value.ht);
            }
        } else if (Z_TYPE_P(retval) == IS_OBJECT) {
            zend_class_entry* ce;
            ce = Z_OBJCE_P(retval);
//            ce = zend_get_class_entry(retval);

            if (strcmp(ce->name, (char*)"Sample") == 0) {
                char* propname;
                propname = (char*)"message";

                zval* property;
                property = zend_read_property(ce, retval, propname, strlen(propname),0);

                cout << Z_STRVAL_P(property) << endl;
            }
        }
    } zend_end_try();

    PHP_EMBED_END_BLOCK();

    return 0;
}
