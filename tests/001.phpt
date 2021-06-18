--TEST--
Check if aspect is loaded
--SKIPIF--
<?php
if (!extension_loaded('aspect')) {
    echo 'skip';
}
?>
--FILE--
<?php
echo 'The extension "aspect" is available';
?>
--EXPECT--
The extension "aspect" is available
