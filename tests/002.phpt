--TEST--
Testing creation of new document
--SKIPIF--
<?php if (!extension_loaded("ps")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 
$psdoc = ps_new();
echo "$psdoc\n";
ps_delete($psdoc);
echo "Done\n";
?>
--EXPECT--
Resource id #%d
Done
