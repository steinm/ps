--TEST--
Testing hyphenation
--SKIPIF--
<?php if (!extension_loaded("ps")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 
$word = "Koordinatensystem";
$psdoc = ps_new();
ps_set_parameter($psdoc, "hyphendict", "tests/hyph_de.dic");
$hyphens = ps_hyphenate($psdoc, $word);
for($i=0; $i<strlen($word); $i++) {
	echo $word[$i];
	if(in_array($i, $hyphens))
		echo "-";
}
ps_delete($psdoc);
?>
--EXPECT--
Ko-ordi-na-ten-sys-tem
