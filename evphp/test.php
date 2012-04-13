<?php
    header('Content-Type: text/plain');

    echo "hoge";

    var_dump(array("hoge", "fuga", "foobar"));

/*
    class Sample {
        public function __construct() {
            $this->message = 'hoge fuga foobar';
        }

        public function getMessage() {
            return "ゲッツ!!";
        }
    }

    return new Sample();

    $cb = function() {
        return array("hoge fuga foobar");
    };

    return array("hoge", "fuga", 1, array("foobar"), $cb);



    return function() {
        var_dump(array("hoge", "fuga"));
    };
*/
?>
