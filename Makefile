# **************************************************************************** #
#    Makefile                                                                  #
# **************************************************************************** #

NAME	:=	webserv

VPATH	:=	srcs

SRCS	:=	main.cpp	\
			colors.cpp	\
			Harl.cpp	\
			Server.cpp	\
			Client.cpp

OBJS_DIR:=	objs
OBJS	:=	$(addprefix $(OBJS_DIR)/, $(SRCS:.cpp=.o))


INCL_DIR:=	./includes

CC		:=	c++
CFLAGS	:=	-Wall -Wextra -Werror
IFLAGS	:=	-I $(INCL_DIR)

ifdef STD
CFLAGS	+=	-std=$(STD)
else
CFLAGS	+=	-std=c++98
endif

RM		:=	rm -rf

# **************************************************************************** #

NC				:=	"\\e[0m"
THEME_COLOR		:=	"\\e[32m"

MAKEFILE_NAME	:=	"make@ex00"

STATUS_MSGS	:=	OK!						\
				working_really_hard...	\
				doing_my_best...		\
				working_harder...		\
				please_be_patient...

# **************************************************************************** #

# The total load of a make task #
LOAD		:=	0

ifndef CALL_MAKE
COMPIL_LOAD := $(shell $(MAKE) $(MAKECMDGOALS) -n SERIOUS=TRUE CALL_MAKE=0\
	| grep '^gcc' | wc -l)
endif

# Track the progress of a make task #
PROGRESS	:=	0

# String containing the longest progress bar possible (100%) #
MAX_PROG_BAR	:=	"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"

# Variable limiting the maximum size of the progress bar (100%) #
MAX_PROG_LENGTH	:=	40

# **************************************************************************** #

all:	$(NAME)

$(NAME):	$(OBJS) | print_flags
	$(eval CMD := $(CC) $(CFLAGS) $(IFLAGS) -o $@ $^)
	$(call custom_loading_command, $(CMD), "$(THEME_COLOR)Compiling $@$(NC)")

$(OBJS_DIR)/%.o:	%.cpp | $(OBJS_DIR) get_obj_load print_flags
	$(eval CMD := $(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<)
	$(call custom_loading_command, $(CMD), "$(THEME_COLOR)Compiling $@$(NC)")
	$(eval PROGRESS := $(shell echo $$(( $(PROGRESS) + 1 ))))
	$(call put_loading, $(PROGRESS), $(LOAD), $(MAX_PROG_LENGTH))

$(OBJS_DIR):
	$(eval CMD := mkdir $(OBJS_DIR))
	$(call custom_command, $(CMD), "Created the missing $(OBJS_DIR) for you. "\
		"Be careful next time.")

ifneq ($(shell ls $(OBJS_DIR)/*.o 2> /dev/null | wc -l), 0)
clean:
	$(eval CMD := $(RM) $(OBJS))
	$(call custom_command, $(CMD), "Yeet and delete $(OBJS) !")
endif

fclean:	clean
ifneq ($(shell [ -f $(NAME) ] && echo 1 || echo 0), 0)
	$(eval CMD := $(RM) $(NAME))
	$(call custom_command, $(CMD), "Goodbye $(NAME) !")
endif

re:	fclean all

# * SETUP RULES ************************************************************** #

# Calculate the amount of object files to compile for the loading bar #
get_obj_load:
ifndef CALL_MAKE
	$(eval LOAD := $(shell $(MAKE) $(MAKECMDGOALS) -n SERIOUS=TRUE CALL_MAKE=0\
		| grep '^$(CC)' | grep -v '$(NAME)' | wc -l))
	$(eval PROGRESS := 0)
endif

# Display the flags currently used for compilation #
print_flags:
ifneq ($(COMPIL_LOAD), 0)
	$(call custom_command, true, "compilation flags: $(CFLAGS)")
else ifneq (,$(findstring re, $(MAKECMDGOALS)))
	$(call custom_command, true, "compilation flags: $(CFLAGS)")
endif

# **************************************************************************** #

ifneq ($(SERIOUS), TRUE)
 define custom_command
	$(eval command = $(1))
	$(eval custom_command_name = $(2))
	@echo "$(MAKEFILE_NAME): $(THEME_COLOR)$(custom_command_name)$(NC)";\
	$(command)
 endef

 define custom_loading_command
	$(eval command = $(1))
	$(eval custom_command_name = $(2))
	$(eval start_status := $(subst _, ,\
		$(word $(shell shuf -i 2-$(words $(STATUS_MSGS)) -n 1), $(STATUS_MSGS))))
	$(eval end_status := $(subst _, , $(word 1, $(STATUS_MSGS))))
	@echo "$(MAKEFILE_NAME): [$(start_status)] $(custom_command_name)";		\
	$(command); 															\
	tmp=$$?;																\
	if [ $$tmp -eq 0 ]; then												\
		echo -n "\e[A\e[K$(MAKEFILE_NAME): [$(end_status)] ";				\
		echo "$(custom_command_name)";										\
	else																	\
		echo "$(MAKEFILE_NAME): [ouch] $(custom_command_name)";				\
	fi;																		\
	exit $$tmp
 endef

# Function to display a loading bar with a given progress, total load and length #
 define put_loading
	$(eval progress := $(1))
	$(eval load := $(2))
	$(eval bar_length := $(3))
	@relative_progress=$$(( $(progress) * $(bar_length) / $(load) ));			\
	status="$(progress) / $(load)";												\
	if [ $(progress) -ne $(load) ]; then										\
		echo -n "\n";															\
	else																		\
		$(eval end_status := $(subst _, , $(word 1, $(STATUS_MSGS)))) true;		\
		status=$(end_status);													\
	fi;																			\
	echo -n "\e[K$(MAKEFILE_NAME): [$$status] ";								\
	printf "[%.*s" $$relative_progress "$(MAX_PROG_BAR)";						\
	printf "%-*s]\n" $$(( $(bar_length) - $$relative_progress )) "";			\
	if [ $(progress) -ne $(load) ]; then										\
		echo -n "\e[2A";														\
	fi
 endef

else
 define custom_command
	$(1)
 endef
 define custom_loading_command
	$(1)
 endef
endif

# **************************************************************************** #

.PHONY: all clean fclean re
