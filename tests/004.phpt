--TEST--
Testing setting and getting values
--SKIPIF--
<?php if (!extension_loaded("ps")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 
$psdoc = ps_new();
ps_set_value($psdoc, "anyname", 0.125);
echo ps_get_value($psdoc, "anyname", 0.0)."\n";
ps_delete($psdoc);
echo "Done\n";
?>
--EXPECT--
0.125
Done
