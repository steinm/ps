--TEST--
Getting version of pslib
--SKIPIF--
<?php if (!extension_loaded("ps")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 
$psdoc = ps_new();
echo ps_get_parameter($psdoc, "dottedversion", 0.0)."\n";
ps_delete($psdoc);
echo "Done\n";
?>
--EXPECTF--
%d.%d.%d
Done
