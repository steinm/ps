<?php

$ps = ps_new();

if (!ps_open_file($ps, "image.ps")) {
	print "Cannot open PostScript file\n";
	exit;
}

ps_set_parameter($ps, "warning", "true");

ps_set_info($ps, "Creator", "image.php");
ps_set_info($ps, "Author", "Uwe Steinmann");
ps_set_info($ps, "Title", "Many examples");

ps_begin_page($ps, 596, 842);
$psfont = ps_findfont($ps, "Helvetica", "", 0);
ps_setfont($ps, $psfont, 12.0);
ps_set_value($ps, "leading", 16);
ps_show_boxed($ps, "The same image in its original size (left) and scaled by a factor of 10.0 (below).", 200, 700, 180, 80, "left", NULL);
$psimage = ps_open_image_file($ps, "png", "debian.png", "", 0);
ps_place_image($ps, $psimage, 50, 50, 10.0);
ps_place_image($ps, $psimage, 50, 730, 1.0);
ps_end_page($ps);

ps_close($ps);
ps_delete($ps);
?>
