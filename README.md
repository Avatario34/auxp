# Aux plugin

'Auxp' is a plugin for grblHAL as of April 18, 2023 Version 0.2

This plugin keeps track of changes of the various digital Aux output ports in grblHAl. 

It extends grblHAL's realtime report by adding the element `|Dout:<signals>`.

`<signals>` is a string containing the numbers `0` to `7` if the corresponding digital Aux output port is active.

Example: `|Dout:02` means, that the digital Aux output ports `0` and `2` are activated.
