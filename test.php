<?php
    class Sample {
        public function __construct() {
            $this->message = 'hoge fuga foobar';
        }

        public function getMessage() {
            return $this->message;
        }
    }

    return new Sample();

//    return array("hoge", "fuga");
?>
