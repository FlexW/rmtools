macroScript uninstall category:"Extended Saving" tooltip:"Uninstall"
(
	on execute do
	(
		if ( querybox "Do you really want to remove the \"Extended Saving\" menu?" title:"Extended Saving" ) == true then
		(
			callbacks.removeScripts id:#Extended_Saving
			menuman.unRegisterMenu ( menuman.findmenu "Extended Saving" )
			local tokenMenu = menuMan.findMenu "Extended Saving By Default"
			if tokenMenu != undefined then menuman.unRegisterMenu tokenMenu
			menuman.updateMenuBar()
		)
	)
) 