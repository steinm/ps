--TEST--
Testing setting and getting parameters
--SKIPIF--
<?php if (!extension_loaded("ps")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 
$psdoc = ps_new();
ps_set_parameter($psdoc, "anyname", "anyvalue");
echo ps_get_parameter($psdoc, "anyname", 0.0)."\n";
ps_delete($psdoc);
echo "Done\n";
?>
--EXPECT--
anyvalue
Done
