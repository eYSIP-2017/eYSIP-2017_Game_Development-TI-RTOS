# invoke SourceDir generated makefile for game.pem4f
game.pem4f: .libraries,game.pem4f
.libraries,game.pem4f: package/cfg/game_pem4f.xdl
	$(MAKE) -f /Users/AxRox/TIVA_Workspace/BREAKOUT_ALPHA/src/makefile.libs

clean::
	$(MAKE) -f /Users/AxRox/TIVA_Workspace/BREAKOUT_ALPHA/src/makefile.libs clean

