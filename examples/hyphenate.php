<?php
if($_SERVER["argc"] < 3) {
	echo "Usage: ".$_SERVER["SCRIPT_FILENAME"]." language word1 word2 ...\n";
	echo "  language can be 'de', 'en'\n";
}
if(!in_array($_SERVER["argv"][1], array("de", "en"))) {
	echo "lanuage must be 'en' or 'de'\n";
}

$ps = ps_new();
ps_set_parameter($ps, "hyphendict", "hyph_".$_SERVER["argv"][1].".dic");
for($i=2; $i<$_SERVER["argc"]; $i++) {
	$word = $_SERVER["argv"][$i];
	$tmp = ps_hyphenate($ps, $word);
	if($tmp) {
		print_r($tmp);
		$start = 0;
		foreach($tmp as $pos) {
			echo substr($word, $start, $pos-$start+1)."-";
			$start = $pos+1;
		}
		echo substr($word, $start);
		echo "\n";
	}
}

ps_delete($ps);
?>
