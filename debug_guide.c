/**
 * @page debug_guide VSCode Debugging Quick Guide
 * @tableofcontents
 * @htmlonly
 * <div id="author_date">
 * tgrekov UNPUBLISHED 2023/11/13
 * </div>
 * @endhtmlonly
 * 
 * THIS ARTICLE IS INCOMPLETE AND IS CURRENTLY UP FOR PROOFREADING PRIOR
 * TO COMPLETION  
 * IT IS STILL BEING WRITTEN
 * 
 * Being able to visualize how your code runs is a crucial step to resolving
 * issues in your programs, which otherwise could result in hours wasted
 * struggling to track down an obscure problem.
 * 
 * Set up properly, debugging C in VSCode will allow you to step through your
 * code and pause at designated points before continuing, or even go line by
 * line to give you a chance to observe how what you've written interacts with
 * inputs and variables.
 * 
 * @section flags Compiler Flags
 * To provide our debugger with the information it needs, we will want to
 * replace our default 3 compiler flags <tt>-Wall -Wextra -Werror</tt> with
 * a few that are more tailored for this task.
 * 
 * @subsection flags__why_replace Wait, Why Replace Our Flags?
 * The <tt>-Wall -Wextra -Werror</tt> can prevent certain issues from being
 * traced properly, so they may still be detected, but you may be missing out
 * on information regarding their origin.
 * 
 * @subsection flags__g -g
 * The @p -g flag tells the compiler to generate extra information that can be
 * used by our debugger to display the names, addresses, and contents of
 * variables, visualize structures, etc.
 * 
 * @subsection flags__fsan -fsanitize (Optional)
 * The @p -fsanitize flag tells the compiler to enable different "sanitizers"
 * which can catch errors and help the user diagnose where and how the issue
 * originated. If you do not wish to have these issues diagnosed and
 * highlighted for you, skip ahead to @ref compiling "Compiling", and remove
 * <tt>-fsanitize=address,undefined,integer</tt> from the @p DEBUG_FLAGS.
 * 
 * @subsubsection flags__fsan_asan -fsanitize=address
 * The @p fsanitize=address flag enables the
 * <a href="https://clang.llvm.org/docs/AddressSanitizer.html">
 * AddressSanitizer</a>, which detects memory accesses outside of allocated
 * areas, use of memory after it has been freed, use of variables outside of
 * their local scope, double and invalid free. To enable the use of the
 * after-return/scope detector we will need to set the @p ASAN_OPTIONS
 * environment variable, but we will get to this part later.
 * 
 * @subsubsection flags__fsan_ubsan -fsanitize=undefined
 * The @p -fsanitize=undefined flag enables the
 * <a href="https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html">
 * UndefinedBehaviorSanitizer</a>, which detects many different issues, but
 * this guide will highlight the ones you may encounter within your first few
 * projects:
 * + @p alignment Use of a misaligned pointer or creation of a misaligned
 * reference
 * + @p builtin Passing invalid values to compiler builtins
 * + @p enum Setting the value of an enumerated type which that enum cannot
 * represent
 * + @p function Calling a function through a function pointer of the wrong
 * type
 * + @p implicit-signed-integer-truncation and
 * @p implicit-unsigned-integer-truncation Implicit conversion from an integer
 * type with a larger bit width to one with a smaller bit width, when the
 * conversion results in data loss
 * + @p implicit-integer-sign-change Implicit conversion between integer types
 * when the conversion results in the sign of the integer changing. (Positive
 * to negative, and negative to positive or zero)
 * + @p integer-divide-by-zero Integer division by zero
 * + @p null Use of a null pointer or creation of a null reference
 * + @p pointer-overflow Performing pointer arithmetic which overflows, or
 * where the old and new pointer are both null
 * + @p signed-integer-overflow and @p unsigned-integer-overflow When the
 * result of performed math cannot be contained in the type used
 * 
 * To enable the above checks, we will use the <tt>undefined,integer</tt> groups
 * 
 * @section compiling Compiling
 * So we've discovered some useful compiler flags to assist us in debugging
 * our program. We can invoke them manually, with the full command, create a
 * new compiler alias, or add a @p debug rule to our Makefile.
 * 
 * @subsection compiling__makefile_example Example Makefile Debug Rule
 * If we decide to add a @p debug rule to our Makefile you can create new
 * variables for your debugging flags and the name of the executable compiled
 * with them
 * 
 * @code
 * DEBUG_NAME = debug.out
 * DEBUG_FLAGS = -g -fsanitize=address,undefined,integer
 * @endcode
 * 
 * And the rules to compile with these flags
 * @code
 * 
 * $(DEBUG_OBJ_DIR)%.o: $(SRC_DIR)%.c $(HEADERS)
 *   @mkdir -p $(DEBUG_OBJ_DIR)$(subst $(SRC_DIR),,$(dir $<))
 *   @echo Building debug $<
 *   @$(CC) $(DEBUG_FLAGS) -c $< -o $@
 * 
 * $(DEBUG_NAME): $(DEBUG_OBJS)
 *   @echo Compiling debug executable
 *   @$(CC) $(DEBUG_FLAGS) $(DEBUG_OBJS) -o $@
 *   @echo Debug done
 *
 * debug: debug.out
 * @endcode
 * 
 * Make sure to appropriately modify your @p clean rule and @p fclean rules
 * 
 * @code
 * clean:
 *   @echo Removing $(OBJ_DIR), $(DEBUG_OBJ_DIR), and .bonus
 *   @rm -rf $(OBJ_DIR) $(DEBUG_OBJ_DIR) .bonus
 *
 * fclean: clean
 *   @echo Removing $(NAME) and $(DEBUG_NAME)
 *   @rm -f $(NAME) $(DEBUG_NAME)
 * @endcode
 * 
 * For more on Makefiles, see
 * @ref makefiles "Spectacular(-ish) Makefiles and How to Make Them"
 * 
 * @section vscode_setup VSCode Debugging Setup
 * Now that we've learned how to compile an executable with debug information
 * and sanitizers, it's time to set up our debugger. 
 * 
 * @subsection vscode_setup__codelldb The CodeLLDB Extension
 * While we could use the debugging provided by the
 * <a href="https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools">
 * C/C++</a> extension by Microsoft that is likely already installed, it either
 * lacks or doesn't have C support for certain conveniences, such as pausing
 * on variable change and / or access from the sidebar menu.
 * 
 * To install the
 * <a href="https://marketplace.visualstudio.com/items?itemName=vadimcn.vscode-lldb">
 * CodeLLDB</a> extension:
 * -# Select the "Extensions" tab in your sidebar:
 * @imageurl{https://raw.githubusercontent.com/microsoft/vscode-icons/main/icons/dark/extensions.svg}
 * -# Search for "CodeLLDB"
 * -# Select Install
 * 
 * @subsection vscode_setup__codelldb_config Configuring the Debugger
 * -# In the sidebar, select the "Run and Debug" icon:
 * @imageurl{https://raw.githubusercontent.com/microsoft/vscode-icons/main/icons/dark/debug-alt.svg}
 * -# Then select "create a launch.json file".
 * -# When prompted to select a workspace folder to create the file in, select
 * your project folder.
 * -# When prompted to select a debugger, select LLDB.
 * -# Set the @p program property to <tt>${workspaceFolder}/debug.out</tt>
 * -# To automatically break when the program starts, create a new key called
 * @p stopOnEntry with @p true as the value.
 * -# To set the @p ASAN_OPTIONS environment variable we talked about earlier,
 * create a new key called @p env with an empty object (<tt>{}</tt>) as the
 * value.
 * @code
 * "stopOnEntry": true,
 * "env": {
 * 
 * },
 * @endcode
 * -# Create a key inside the @p env object with the name @p ASAN_OPTIONS and
 * the value <tt>detect_stack_use_after_return=1</tt>.  
 * @code
 * "env": {
 *   "ASAN_OPTIONS": "detect_stack_use_after_return=1"
 * ],
 * @endcode
 * -# To tell the debugger that we are targetting C, create a new key called
 * @p sourceLanguages with array (<tt>[]</tt>) containing the value @p c.
 * @code
 * "sourceLanguages": [
 *   "c"
 * ],
 * @endcode
 * -# If you want the Debug Console to automatically open when you start
 * debugging, add the key @p internalConsoleOptions with the value
 * @p openOnSessionStart (Thank you jmertane!)
 * @code
 * "internalConsoleOptions": "openOnSessionStart"
 * @endcode
 * 
 * @subsection vscode_setup__tasks VSCode Tasks (Optional)
 * If you would like for VSCode to automatically run your compilation command
 * for you when you debug, follow these steps. If you skip this section, make
 * sure to omit @p preLaunchTask from your launch.json.
 * 
 * -# Press @p CTRL + @p SHIFT + @p P to open the command palette.
 * -# Type "Tasks"
 * -# Select "Tasks: Configure Task"
 * -# Select "Create tasks.json file from template"
 * -# Select "Others"
 * -# Change the value of @p label to something like <tt>Build debug executable</tt>
 * -# Change the value of @p command to <tt>make debug</tt> or whatever command
 * is relevant to how you set up your compilation step.  
 * Your tasks.json should now look something like this
 * @code
 * {
 *   "version": "2.0.0",
 *   "tasks": [
 *     {
 *       "label": "Build debug executable",
 *       "type": "shell",
 *       "command": "make debug"
 *     }
 *   ]
 * }
 * @endcode
 * -# Now back in your launch.json, add the key @p preLaunchTask and set the
 * value to the label of the task you created.
 * @code
 * "preLaunchTask": "Build debug executable"
 * @endcode
 * 
 * @subsection vscode_setup__complete_launch_json The Completed launch.json
 * After completing all of the above steps, your launch.json should look
 * something like this
 * @code
 * {
 *   "version": "0.2.0",
 *   "configurations": [
 *     {
 *       "type": "lldb",
 *       "request": "launch",
 *       "name": "Debug",
 *       "program": "${workspaceFolder}/debug.out",
 *       "args": [],
 *       "cwd": "${workspaceFolder}",
 *       "stopOnEntry": true,
 *       "env": {
 *         "ASAN_OPTIONS": "detect_stack_use_after_return=1"
 *       },
 *       "sourceLanguages": [
 *         "c"
 *       ],
 *       "internalConsoleOptions": "openOnSessionStart",
 *       "preLaunchTask": "Build debug executable"
 *     }
 *   ]
 * }
 * @endcode
 * 
 * @subsection vscode_setup__settings_json settings.json (Optional)
 * A few small things to configure for convenience.
 * 
 * CodeLLDB enters its
 * <a href="https://github.com/vadimcn/codelldb/blob/master/MANUAL.md#disassembly-view">
 * Disassembly View</a> when it is missing sufficient debug information to show
 * the user what is occurring. This can be useful, but it is likely too advanced
 * for us to use yet, so I would recommend disabling it.
 * 
 * -# Select the "File Explorer" tab in the sidebar
 * @imageurl{https://raw.githubusercontent.com/microsoft/vscode-icons/main/icons/dark/files.svg}
 * -# Open the .vscode directory in your workspace. It should contain the 
 * launch.json and tasks.json files you created earlier.
 * -# Create a new file called settings.json, containing an object <tt>{}</tt>.
 * -# Create the key <tt>lldb.showDisassembly</tt> with the value @p never.
 * @code
 * {
 *   "lldb.showDisassembly": "never"
 * }
 * @endcode
 * 
 * @section debugging VSCode Debugging With LLDB
 * Now we are ready to inspect any part of our project.
 * 
 * @subsection debugging__preparing_debug Preparing Your Code
 * Now we must find what exactly we want to debug.  
 * A simple example to practice using our debugger on would be
 * inspecting how your
 * <a href="https://ft-spellbook.github.io/tgrekov-libft/ft__strlen_8c.html">
 * ft_strlen</a> executes.
 * 
 * Create a main that calls your
 * <a href="https://ft-spellbook.github.io/tgrekov-libft/ft__strlen_8c.html">
 * ft_strlen</a>, stores the return value in a variable, and then increments
 * it by one, like so:
 * @code
 * int	main(void)
 * {
 *   size_t	len;
 * 
 *   len = ft_strlen("Test");
 *   len++;
 * }
 * @endcode
 * 
 * @subsection debugging__start_debugger Starting the Debugger
 * -# Select the "Run and Debug" tab in the sidebar
 * @imageurl{https://raw.githubusercontent.com/microsoft/vscode-icons/main/icons/dark/debug-alt.svg}
 * -# Start debugging by selecting the Start button at the top of the menu
 * @imageurl{https://raw.githubusercontent.com/microsoft/vscode-icons/main/icons/dark/debug-start.svg}
 * -# If you have @p stopOnEntry enabled, you should immediately see a warning
 * in your Debug Console that the debugger received a signal @p SIGSTOP.
 * 
 * You are now ready to explore your program.
 * 
 * @subsection debugging__breakpoints Breakpoints
 * Breakpoints are how you can communicate to your debugger where in your code
 * you want it to pause ("break"). You do not need to recompile to set or
 * remove breakpoints, as they do not affect the program itself, but are
 * instead provided to the debugger to check against debug information
 * preserved in the executable to determine where to break.
 * 
 * Breakpoints in VSCode are indicated with a red dot
 * @htmlonly
 * <svg width="16" height="16" viewBox="0 0 16 16" fill="none" xmlns="http://www.w3.org/2000/svg">
 *   <path d="M8 4C8.36719 4 8.72135 4.04818 9.0625 4.14453C9.40365 4.23828 9.72135 4.3724 10.0156 4.54688C10.3125 4.72135 10.582 4.93099 10.8242 5.17578C11.069 5.41797 11.2786 5.6875 11.4531 5.98438C11.6276 6.27865 11.7617 6.59635 11.8555 6.9375C11.9518 7.27865 12 7.63281 12 8C12 8.36719 11.9518 8.72135 11.8555 9.0625C11.7617 9.40365 11.6276 9.72266 11.4531 10.0195C11.2786 10.3138 11.069 10.5833 10.8242 10.8281C10.582 11.0703 10.3125 11.2786 10.0156 11.4531C9.72135 11.6276 9.40365 11.763 9.0625 11.8594C8.72135 11.9531 8.36719 12 8 12C7.63281 12 7.27865 11.9531 6.9375 11.8594C6.59635 11.763 6.27734 11.6276 5.98047 11.4531C5.6862 11.2786 5.41667 11.0703 5.17188 10.8281C4.92969 10.5833 4.72135 10.3138 4.54688 10.0195C4.3724 9.72266 4.23698 9.40365 4.14063 9.0625C4.04688 8.72135 4 8.36719 4 8C4 7.63281 4.04688 7.27865 4.14063 6.9375C4.23698 6.59635 4.3724 6.27865 4.54688 5.98438C4.72135 5.6875 4.92969 5.41797 5.17188 5.17578C5.41667 4.93099 5.6862 4.72135 5.98047 4.54688C6.27734 4.3724 6.59635 4.23828 6.9375 4.14453C7.27865 4.04818 7.63281 4 8 4Z" fill="#ff0000"/>
 * </svg>
 * @endhtmlonly
 * .<br>You can set breakpoints in VSCode by clicking to the left of a line
 * number.
 * 
 * @subsubsection debugging__breakpoints_moving Why Are My Breakpoints Moving?
 * If you set a breakpoint on a line that the debugger cannot break on, such
 * as an empty line or a variable declaration, it will be moved to the
 * nearest valid location.
 * 
 * @image html breakpoint_moving.gif width=300px
 * 
 * @subsection debugging__navigation Stepping Through Your Program
 * Now that we know how to tell the debugger to stop at certain places, we need
 * to start navigating through our program.
 * 
 * @subsubsection debugging__navigation_navigation_bar Navigation Bar
 * The navigation bar at the top of the screen has the necessities for traversing
 * our code.
 * 
 * @image html navigation_bar.png width=500px
 * 
 * Continue @imageurl{https://raw.githubusercontent.com/microsoft/vscode-icons/main/icons/dark/debug-continue.svg}
 * <br>Causes the debugger to run your code until it encounters a breakpoint or
 * a break is triggered for another reason.  
 * @image html breakpoint_continue.gif width=500px
 * 
 * Step Over @imageurl{https://raw.githubusercontent.com/microsoft/vscode-icons/main/icons/dark/debug-step-over.svg}
 * <br>Steps over the current line. If there is a function call, it calls the
 * function and moves on.  
 * @image html step_over.gif width=500px
 * 
 * Step Into @imageurl{https://raw.githubusercontent.com/microsoft/vscode-icons/main/icons/dark/debug-step-into.svg}
 * <br>If there is not a function call on the current lines, acts like Step Over. If
 * a function call is present, enters (steps into) the function.  
 * @image html step_into.gif width=500px
 * 
 * Step Out @imageurl{https://raw.githubusercontent.com/microsoft/vscode-icons/main/icons/dark/debug-step-out.svg}
 * <br>Steps back out of a function call, allowing it to complete.
 * @image html step_out.gif width=500px
 * 
 * Restart @imageurl{https://raw.githubusercontent.com/microsoft/vscode-icons/main/icons/dark/debug-restart.svg}
 * <br>Restarts the entire debug process, including tasks.
 * 
 * Stop @imageurl{https://raw.githubusercontent.com/microsoft/vscode-icons/main/icons/dark/debug-stop.svg}
 * <br>Stop debugging and exit.
 * 
 * @subsubsection debugging__navigation_jump Jump to Cursor
 * Right-clicking on a line in your code, you should have the option at the
 * bottom of the context menu to "jump" to the cursor position. 
 * @image html cursor_jump_to.png width=500px
 * 
 * Jumping will skip straight to the cursor, ignoring any code between the
 * current position and the destination. You can jump both forward and
 * backward.  
 * Jumping may require you to pause and unpause in the navigation bar.
 * 
 * @subsubsection debugging__navigation_run Run to Cursor
 * Right-clicking on a line in your code, you should have the option at the
 * bottom of the context menu to "run" until the cursor position.
 * @image html cursor_run_to.png width=500px
 * 
 * This will execute the code normally and break when the cursor position is
 * reached.
 * 
 * @subsection debugging__data_breakpoints Data Breakpoints
 * Sometimes called watchpoints, data breakpoints can break when data is
 * read, when data is modified, or either (accessed).
 * 
 * To set a data breakpoint in VSCode, set a breakpoint to the earliest part
 * of your code where that data is available and is in a context where you
 * expect it to be observed or modified.
 * 
 * Let's say we want to observe the counter in our
 * <a href="https://ft-spellbook.github.io/tgrekov-libft/ft__strlen_8c.html">
 * ft_strlen</a>. We can set a breakpoint where that counter is initialized,
 * and then use the
 * Continue @imageurl{https://raw.githubusercontent.com/microsoft/vscode-icons/main/icons/dark/debug-continue.svg}
 * movement key to run until we hit that breakpoint.
 * @image html reach_initialization_breakpoint.gif width=500px
 * 
 * We can now see the local variables for this context in the sidebar. To set
 * a data breakpoint on one of these variables, we can right-click it and
 * select one of the following:
 * 
 * @subsubsection debugging__data_breakpoints__change Break on Value Change
 * The debugger will break every time the value we selected is modified.
 * @image html data_change_breakpoint.gif width=500px
 * In the breakpoints list in the bottom portion of the sidebar, we can see
 * our data breakpoint and the letter "W" for "write".
 * 
 * @subsubsection debugging__data_breakpoints__read Break on Value Read
 * The debugger will break every time the value we selected is retrieved.
 * @image html data_read_breakpoint.gif width=500px
 * In the breakpoints list in the bottom portion of the sidebar, we can see
 * our data breakpoint and the letter "R" for "read".
 * 
 * @subsubsection debugging__data_breakpoints__access Break on Value Access
 * The debugger will break every time the value we selected is modified or
 * retrieved.
 * @image html data_access_breakpoint.gif width=500px
 * In the breakpoints list in the bottom portion of the sidebar, we can see
 * our data breakpoint and the letter "A" for "access".
 * 
 * @subsection debugging__special_breakpoints Special Breakpoints
 * We can add a breakpoint in the loop of our
 * <a href="https://ft-spellbook.github.io/tgrekov-libft/ft__strlen_8c.html">
 * ft_strlen</a> to demonstrate the following special breakpoints:
 * @image html conditional_breakpoint_pre-set.png width=300px
 * 
 * @subsubsection debugging__special_breakpoints__conditional Conditional Breakpoints
 * We can set breakpoints that only break when specific conditions are met.
 * 
 * And then edit it to set the condition <tt>i == 2</tt>:
 * @image html conditional_breakpoint_set.gif width=500px
 * 
 * If we then use the Continue @imageurl{https://raw.githubusercontent.com/microsoft/vscode-icons/main/icons/dark/debug-continue.svg}
 * movement key, @p i will increment until it reaches the value @p 2.
 * @image html conditional_breakpoint_hit.gif width=500px
 * 
 * @subsubsection debugging__special_breakpoints__hit_count Hit Count Breakpoints
 * It is also possible to create breakpoints that only break after they have
 * been hit a certain number of times.
 * 
 * We can set the Hit Count to @p 6:
 * @image html breakpoint_hit_count_set.gif width=500px
 * 
 * In which case the debugger will only break in the second call of
 * <a href="https://ft-spellbook.github.io/tgrekov-libft/ft__strlen_8c.html">
 * ft_strlen</a>, where we pass "Test2" as @p str, since the breakpoint will
 * have been hit 6 times when we get to the second character:
 * @image html breakpoint_hit_count_run.png width=500px
 * 
 * @subsubsection debugging__special_breakpoints__log Log Message Breakpoints
 * You can also make breakpoints solely for logging information to the Debug
 * Console.
 * 
 * You can format the log message by including @ref debug__expr "expressions"
 * inside of @p {} braces:
 * @image html breakpoint_log_set.gif width=500px
 * 
 * The resulting output:
 * @image html breakpoint_log_run.png width=500px
 * 
 * @subsection debugging__combining_breakpoint_properties Combining Breakpoint Properties
 * We can even combine multiple of the above, such as making a breakpoint that
 * only logs to the Debug Console when the index is an even number:
 * @image html breakpoint_condition_log_set.gif width=500px
 * 
 * The resulting output:
 * @image html breakpoint_condition_log_run.png width=500px
 * 
 * @subsection debugging__debug_console The Debug Console
 * The Debug Console allows you to interact with the debugger outside of the
 * graphical menus provided by VSCode, such as issuing commands, as well as
 * viewing the debugger's feedback on your actions and various errors and
 * warnings raised by your sanitizers.
 * 
 * @subsection debugging__expr The expression Command
 * The expression command allows you to evaluate C expressions in the current
 * context of the program.  
 * Expressions support most C syntax with the notable exception of control
 * structures like if, else, while, etc.  
 * The return value of your expression will be stored in an incrementally-named
 * @ref debugging__expr__user-defined_variables "User-Defined Variable", so the
 * first time you run the command, the result will be stored in a variable
 * named @p $0, and the next expression's return value will be stored in @p s1,
 * and so on.
 * 
 * @subsubsection debugging__expr__evaluating_expressions Evaluating Expressions
 * Let's say we have the local variable @p i in the current context, whose
 * value is @p 42.  
 * The command <tt>expr i</tt>
 * Would print the value of @p i and assign it to an automatically created
 * @ref debugging__expr__user-defined_variables "User-Defined Variable":
 * Command         | Output
 * ----------------| -------------------------
 * <tt>expr i</tt> | <tt>(size_t) $0 = 42</tt>
 * 
 * If we want to simply check the value of @p i without doing this, we can use
 * <tt>print</tt>:
 * Command          | Output
 * ---------------- | --------------------
 * <tt>print i</tt> | <tt>(size_t) 42</tt>
 * 
 * <br>We can perform a postfix increment on @p i with an expression like we would
 * in normal C code:
 * Command           | Output
 * ----------------- | -------------------------
 * <tt>expr i++</tt> | <tt>(size_t) $1 = 42</tt>
 * <tt>print i</tt>  | <tt>(size_t) 43</tt>
 * <tt>print $1</tt> | <tt>(size_t) 42</tt>
 * 
 * <br>Or a prefix increment:
 * Command           | Output
 * ----------------- | -------------------------
 * <tt>expr i++</tt> | <tt>(size_t) $2 = 44</tt>
 * <tt>print i</tt>  | <tt>(size_t) 44</tt>
 * <tt>print $1</tt> | <tt>(size_t) 44</tt>
 * 
 * @subsubsection debugging__expr__user-defined_variables User-Defined Variables
 * As we saw previously, using the @p expression command stores the return
 * value of the evaluated expression in a User-Defined Variable, and we can
 * also create and manipulate these variables ourselves.
 * 
 * We declare UDVs in the debugger the same way we would declare variables
 * in C, in the format <tt>type name</tt> and, optionally,
 * <tt>= assignment</tt>.  
 * If a UDV's name is prefixed with a @p $, it persists across all expressions,
 * while one without the @p $ only exists within the current expression.
 * 
 * To demonstrate further how UDVs are like C variables, we can create one
 * called @p $ptr which will hold the address of @p i, and then modify @p i
 * by dereferencing @p $ptr.
 * <table>
 *   <tr>
 *     <th>Command</th>
 *     <th>Output</th>
 *   </tr>
 *   <tr>
 *     <td><tt>expr size_t *$ptr</tt><br><tt>expr $ptr = &len</tt></td>
 *     <td><tt>(size_t *) $3 = 0x00007ffeefbff780</tt></td>
 *   </tr>
 * </table>
 * 
 * We can now modify @p i by dereferencing @p ptr
 * Command                  | Output
 * ------------------------ | -------------------------
 * <tt>expr *$ptr = 24</tt> | <tt>(size_t) $4 = 24</tt>
 * <tt>print i</tt>         | <tt>(size_t) 24</tt>
 * <tt>print $4</tt>        | <tt>(size_t) 44</tt>
 * 
 * @subsection debugging__thread The thread Command
 * The thread command is used to operate in the thread of the current process.
 * For example; if you use the Continue @imageurl{https://raw.githubusercontent.com/microsoft/vscode-icons/main/icons/dark/debug-continue.svg}
 * movement button, you execute <tt>thread continue</tt>.  
 * We have already gone over the graphical versions of the commands
 * <tt>thread continue</tt>, <tt>thread step-over</tt>,
 * <tt>thread step-into</tt>, <tt>thread step-out</tt>,
 * <tt>thread until</tt>, and <tt>thread jump</tt>.
 * 
 * @subsubsection debugging__thread__backtrace thread backtrace
 * The <tt>thread backtrace</tt> command shows the current call stack.  
 * If we run this command while at the breakpoint we created earlier in the
 * @ref debugging__data_breakpoints section, we can expect
 * a result similar to the following:
 * @code
 * * thread #1, queue = 'com.apple.main-thread', stop reason = breakpoint 1.1
 *   * frame #0: 0x000000010000586c debug.out`ft_strlen(str="Test") at ft_strlen.c:32:4
 *     frame #1: 0x00000001000059bb debug.out`main at ft_strlen.c:42:8
 *     frame #2: 0x00007fff78deb3d5 libdyld.dylib`start + 1
 *     frame #3: 0x00007fff78deb3d5 libdyld.dylib`start + 1
 * @endcode
 * From this backtrace, we can interpret the following:
 * - We are currently inside of the
 * <a href="https://ft-spellbook.github.io/tgrekov-libft/ft__strlen_8c.html">
 * ft_strlen</a> function, at character 4 of line 32 in ft_strlen.c
 * - The function was passed the parameter @p str, which contains the string
 * "Test". 
 * - We entered the function from the main function, at character 8 of line 42
 * in ft_strlen.c
 * - The thread is currently stopped because of breakpoint #1
 * 
 * We can also see some of this information in the Call Stack section in the
 * sidebar:
 * @image html call_stack.png width=290px
 * 
 * @subsubsection debugging__thread__return thread return
 * The <tt>thread return</tt> command prematurely returns from a context,
 * with an optional return value.
 * If we run this command with the value of @p SIZE_MAX
 * @code
 * 18446744073709551615
 * @endcode
 * while at the breakpoint we created earlier in the
 * @ref debugging__data_breakpoints section,
 * our <a href="https://ft-spellbook.github.io/tgrekov-libft/ft__strlen_8c.html">
 * ft_strlen</a> will return the largest value that @p size_t can store:
 * @image html thread_return.gif width=500px
 * 
 * @subsection debugging__ubsan_in_action UndefinedBehaviorSanitizer In Action
 * If we Step Over @imageurl{https://raw.githubusercontent.com/microsoft/vscode-icons/main/icons/dark/debug-step-over.svg}
 * to the next line, which increments @p len, we will overflow it.  
 * Since we enabled the @p unsigned-integer-overflow check in the
 * @ref flags__fsan_ubsan "UndefinedBehaviorSanitizer", we will break and see
 * the reason in our Debug Console:
 * @image html ubsan_overflow.gif width=500px
 * 
 * @subsubsection debugging__ubsan_in_action__intentional_behavior Intentional Behavior
 * Tripping an @ref flags__fsan_ubsan "UndefinedBehaviorSanitizer" check is not
 * always an indication that something is wrong, however, as it can also break
 * on intentional behavior.
 * 
 * My <a href="https://ft-spellbook.github.io/tgrekov-libft/ft__bzero_8c.html">
 * ft_bzero</a> runs in reverse by decrementing @p n until it reaches zero:
 * @code
 * void    ft_bzero(void *s, size_t n)
 * {
 *   while (n--)
 *     ((unsigned char *) s)[n] = 0;
 * }
 * @endcode
 * 
 * This will underflow on the very final condition test, tripping the
 * @p unsigned-integer-overflow check, causing the debugger to break and
 * output <tt>Stop reason: Unsigned integer overflow</tt> in the Debug Console.
 * 
 * In cases such as these, the warning can simply be ignored, and you can move
 * on.
 * 
 * @section afterword Afterword
 * You should now be armed with all the tools and knowledge you need to get
 * started tracking down and fixing issues in your programs, no matter how
 * complex! I hope I have done well to help prepare you for this task, and you
 * are feeling more confident in your abilities.
 * 
 * This article was honestly a fairly large project for me, from learning
 * everything I could about LLDB and the CodeLLDB extension to the detail and
 * finishing touches like the examples and gifs, I've had a lot of fun and
 * seriously enjoyed working on this.
 * 
 * I have done as much research as I could to make sure that I provided the
 * most accurate and readable information I could present.  
 * That being said, if you have found any issues or inaccuracies, parts that
 * could use clarification, or otherwise have any questions, please contact me
 * on Discord at @sova5099 (tgrekov in the HIVE Discord!).
 * 
 * Thank you for coming along on this journey with me!
 * 
 * @subsection afterword__resources Resources
 * You can learn more about LLDB by running the @p help command in the debug
 * console, at the <a href="https://lldb.llvm.org/use/tutorial.html">
 * official tutorial</a>, and at the
 * <a href="https://lldb.llvm.org/use/map.html">
 * official GDB to LLDB command map</a>.
 * 
 * You can learn more about CodeLLDB in the
 * <a href="https://github.com/vadimcn/codelldb/blob/master/MANUAL.md">
 * official manual</a>.
 * 
 * @subsection afterword__lsan LeakSanitizer
 * The version of Clang that Apple provides does not support
 * <a href="https://clang.llvm.org/docs/LeakSanitizer.html">LeakSanitizer</a>
 * because it is not considered mature enough on MacOS. You could get around
 * this by building the LLVM compiler and toolchain from the source, but this
 * is not generally advisable.
 * 
 * Sometime in the near future, there will be an article about Plugging Leaks
 * (most likely using the @p leaks command)!
 */
