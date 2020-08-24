The main file to look at for crucial code is main.cpp
I inserted my physics calculation functions there and i moved integration 
function from body to main to make it easier


Application
Camera
Mesh
and OBJLoader 
are all graphics specific and i didn't parallelise anything in them, but decided to include 
to make sure everything makes sense.
Particle is a child of body and is pretty ambiguous, but this is based on the code
written for physics animation module last year and was extracted from my repostory about half-way through
some coursework, as i only needed a base that draws multiple particles on screen, so i decided to leave
old classes as they are.

