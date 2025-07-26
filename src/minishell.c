/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mubersan <mubersan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 19:51:07 by mubersan          #+#    #+#             */
/*   Updated: 2025/07/23 19:02:04 by mubersan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int			g_exit_status = 0;

static void	process_commands(t_data *data, char *prompt)
{
	data->token = tokenize(prompt, data);

	if (!data->token)
		return ;
	data->cmd = parse_tokens(data->token);
	if (!data->cmd)
	{
		free_tokens(data->token);
		return ;
	}
	data->fds = NULL;
	allocate_heredoc(data->token, data->cmd);
	fill_heredocs_from_tokens(data->token, data->cmd);
	process_heredocs(data->cmd, data);
	execute_all_cmd(data->cmd, data, data->token);
	free_tokens(data->token);
	free_cmd(data->cmd);
}

static int	handle_prompt_input(t_data *data, char *prompt)
{
	if (!prompt)
	{
		ft_putstr_fd("exit\n", 1);
		free_data(data);
		return (1);
	}
	if (*prompt)
		add_history(prompt);
	process_commands(data, prompt);
	free(prompt);
	return (0);
}

static int	initialize_minishell(char **envp, t_data **data)
{
	if (!envp)
		return (1);
	*data = init_data(envp);
	if (!*data)
		return (1);
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, SIG_IGN);
	return (0);
}

static int	run_minishell_loop(t_data *data)
{
	char	*prompt;

	while ((prompt = readline("minishell$ ")))
	{
		handle_sigint_status(data);
		if (handle_prompt_input(data, prompt))
			return (0);
		if (data->should_exit)
			break ;
	}
	return (1);
}

int	main(int ac, char **av, char **envp)
{
	t_data	*data;
	int		exit_code;

	(void)av;
	if (ac != 1)
		return (1);
	if (initialize_minishell(envp, &data))
		return (1);
	run_minishell_loop(data);
	rl_clear_history();
	exit_code = *data->exit->exit;
	free_data(data);
	return (exit_code);
}
