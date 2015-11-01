NAME            =   ft_strace

rwildcard       =   $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2)\
                    $(filter $(subst *,%,$2),$d))

COMPILER        =   gcc

INCLUDE_DIRS    =   ./includes
LIB_DIRS        =
LIB_NAMES       =

CFLAGS          =   -Wall -Wextra -Werror -O3\
                    $(foreach dir, $(INCLUDE_DIRS), -I $(dir))\
                    $(foreach define, $(PP_DEFINES), -D $(define))
LFLAGS          =   -static\
                    $(foreach dir, $(LIB_DIRS), -L $(dir))\
                    $(foreach name, $(LIB_NAMES), -l$(name))

OBJ_DIR         =   objs
SRCS            =   $(call rwildcard, ./srcs, *.c)
OBJS            =   $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))

OBJ_SUB_DIRS    =   $(dir $(OBJS))

all: show_fancy_name
	@$(MAKE) $(NAME)

$(NAME): $(OBJS)
	@echo "$(bold)$(blue)linking		$(green)$@$(reset)"
	@$(COMPILER) $^ $(LFLAGS) -o $@

$(OBJS): | $(OBJ_DIR)

$(OBJ_DIR):
	@$(foreach dir, $(OBJ_SUB_DIRS), mkdir -p $(dir);)

$(OBJ_DIR)/%.o: %.c
	@echo "$(bold)$(cyan)compiling	$(reset)$(white)$(dir $<)$(bold)$(notdir $<)$(reset)"
	@$(COMPILER) $(CFLAGS) -c $< -o $@

depend: .depend

.depend: $(SRCS)
	@rm -f ./.depend
	@$(foreach src, $^, $(COMPILER) $(CFLAGS) -MM -MT $(OBJ_DIR)/./$(src:.c=.o) $(src) >> ./.depend;)

-include .depend

clean: show_fancy_name
	@echo "$(bold)$(red)cleaning	$(white)$(OBJ_DIR)$(reset)"
	@rm -rf $(OBJ_DIR)
	@rm -f ./.depend

fclean: clean
	@echo "$(bold)$(red)removing	$(green)$(NAME)$(reset)"
	@rm -f $(NAME)

re:
	@$(MAKE) fclean
	@$(MAKE) all

show_fancy_name:
	@echo "\n$(bold)$(yellow)==> $(NAME)$(reset)"


# ANSI helpers
ansi        =   \033[$1m

bold        =   $(call ansi,1)
fg_color    =   $(call ansi,38;5;$1)
reset       =   $(call ansi,0)

red         =   $(call fg_color,1)
green       =   $(call fg_color,10)
blue        =   $(call fg_color,27)
cyan        =   $(call fg_color,6)
yellow      =   $(call fg_color,11)
white       =   $(call fg_color,255)
