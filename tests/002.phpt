--TEST--
test1() Basic test
--SKIPIF--
<?php
if (!extension_loaded('aspect')) {
    echo 'skip';
}
?>
--FILE--
<?php
$ret = test1();

var_dump($ret);
?>
--EXPECT--
The extension aspect is loaded and working!
NULL
