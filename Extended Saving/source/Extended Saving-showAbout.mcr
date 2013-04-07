macroScript showAbout category:"Extended Saving" tooltip:"About..."
(
	rollout roAbout "About" width:150
	(
		label lbName "Extended Saving for GMax" align:#left
		label lbVersion "Version 0.8" align:#left
		label lbDate "April 7th 2013" align:#left
		hyperLink hlMe "by Baltram" align:#center address:"mailto:baltram-lielb@web.de" color:( color 0 0 170 ) hoverColor:( color 170 0 0 )
	)
	on execute do
	(
		createDialog roAbout
	)
) 