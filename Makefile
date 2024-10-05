#############################################################################
#                            Name                                            #
#############################################################################

NAME        =   webserv

#############################################################################
#                            Directories                                     #
#############################################################################

MDIR        =	src/
INCDIR      =   includes/
OBJDIR      =   .obj/

#############################################################################
#                            Files                                           #
#############################################################################

MFLS        =	ServerCofig.cpp ConfigParser.cpp TcpServer.cpp main.cpp
MSRC        =   $(addprefix $(MDIR), $(MFLS))
MOBJS		=	$(addprefix $(OBJDIR), $(MFLS:.cpp=.o))
#MOBJS       =   $(MSRC:.cpp=.o)
#MOBJS       :=  $(addprefix $(OBJDIR), $(notdir $(MOBJS)))

#############################################################################
#                            Compiler settings                              #
#############################################################################

CXX				=   c++
CXXFLAGS		=   -std=c++98
CXXFLAGS		+=	-Wall -Werror -Wextra
CXXFLAGS		+=	-MMD -MP
CXXFLAGS		+=	-g
CXXFLAGS		+=  -I$(INCDIR)

#############################################################################
#                            Makefile Dependency                            #
#############################################################################

MKFL		=   Makefile

#############################################################################
#                            Clean up                                       #
#############################################################################

RM          =   rm -rf

#############################################################################
#                            Rules                                          #
#############################################################################

all:            $(NAME)

-include $(MOBJS:.o=.d)

$(OBJDIR)%.o:   $(MDIR)%.cpp
				@mkdir -p $(OBJDIR)
				@$(CXX) $(CXXFLAGS) -c $< -o $@

$(NAME):        $(MOBJS) $(MKFL)
				@echo "\e[35mCompiling\e[0m $(*F)"
				@$(CXX) $(MOBJS) -o $(NAME)
				@echo "\e[32mDone!\e[0m"

clean:
				@$(RM) $(OBJDIR)

fclean:         clean
				@$(RM) $(NAME)

re:             fclean all

.PHONY:         all clean fclean re

# Article about webserver on C++
# https://osasazamegbe.medium.com/showing-building-an-http-server-from-scratch-in-c-2da7c0db6cb7