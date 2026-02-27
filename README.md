ctetris
------

Simply Tetris in the terminal, based off the Tetris seen in the movie 'Tetris,'
i.e. just Tetris. How many times can this guy say Tetris?


# IDEA
Model View Controller type project, so that code inner workings is behind
scenes, and the view can be changed to fit backend (so frontend can change
if needed)

Maybe provide support for threading, so having more than one game at once
through different arrays for each game? Could allow online multiplayer
potential if different arrays/states/contexts are done?

## MODEL
Data is essentially gonna be represented as an array, and potentially put it
in a struct like `ctet_state` which will contain a pointer to the array, the
current gravity, other data that'll be needed.

Handles data validation and is used by the controller to do stuff. Notifies
view once changes have been made.

There's a func like `ctet_update()` that'll just do all the updating based
off accumulated moves every frame or so?

Functions for directly interacting with the data and changing stuff as
needed by the controller.
## VIEW
This'll be through ncurses, though I think initial testing can take place
within the terminal with printing (or make it an option or something I dunno)

Simply formats information from state and doesn't do any data handling/changing.
## CONTROLLER
Handles input from the user and processes it using the model. Will retreive or
update data from the model.

The glue between the user and the rest of the application (taking what happens to the view
and using user input to request changes of the model)

# INCLUDE
this is the public interface people will use with their view to actually create
their own frontend for the system. We return state once requested (so the array),
and the end user figures out how to present that information.

We can also offer the `update()` type command to say 'hey, update the state' and we
can share maybe a certain `context` or something with update to accumulate changes,
like if a move has been made, a hold, or a restore.

void ctet_update() -> update the state based off previous accumulated changes
void ctet_action(enum) -> perform this action on the current state (MOVELEFT, MOVERIGHT, MOVEDOWN, HOLD, RESTORE)
                       -> remember to call ctet_update() after all actions have been made to actually update the given state
