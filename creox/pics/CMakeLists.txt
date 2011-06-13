# XXX: Install gfx resources

install(FILES  DESTINATION share/apps/creox/icons)

#original Makefile.am contents follow:

######## kdevelop will overwrite this part!!! (begin)##########
#
#
#EXTRA_DIST = hi16-action-distortion.png hi16-action-echo.png hi16-action-flanger.png hi16-action-phaser.png hi16-action-preset_chain.png hi16-action-preset_nochain.png hi16-action-tremolo.png hi48-action-sound_card_properties.png sound_card_properties.png 
#
#install-data-local:
#	$(mkinstalldirs) $(kde_datadir)/creox/pics/
#	$(INSTALL_DATA) $(srcdir)/sound_card_properties.png $(kde_datadir)/creox/pics/sound_card_properties.png
#
#uninstall-local:
#	-rm -f $(kde_datadir)/creox/pics/sound_card_properties.png
#
######## kdevelop will overwrite this part!!! (end)############
#creox_icondir = $(kde_datadir)/creox/icons
#creox_icon_ICON = AUTO
