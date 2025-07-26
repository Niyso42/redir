/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_env.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mubersan <mubersan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 18:54:11 by mubersan          #+#    #+#             */
/*   Updated: 2025/07/27 01:22:32 by mubersan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static int count_env_vars(char **envp) {
  int i;

  i = 0;
  while (envp[i])
    i++;
  return (i);
}

static void cleanup_env_allocation(char **env, int up_to) {
  while (up_to-- > 0)
    free(env[up_to]);
  free(env);
}

static int allocate_env_copy(t_data *data, char **envp, int count) {
  int j;

  j = 0;
  while (j < count) {
    data->env->env[j] = ft_strdup(envp[j]);
    if (!data->env->env[j]) {
      cleanup_env_allocation(data->env->env, j);
      data->env->env = NULL;
      return (0);
    }
    j++;
  }
  data->env->env[j] = NULL;
  return (1);
}

void copy_env(t_data *data, char **envp) {
  int count;

  if (!data || !envp)
    return;
  if (!envp[0]) 
    return;
  count = count_env_vars(envp);
  data->env->env = (char **)malloc(sizeof(char *) * (count + 1));
  if (!data->env->env)
    return;
  allocate_env_copy(data, envp, count);
}

int error_handling(int err, t_data *data) {
  if (data)
    free_data(data);

  if (err == 6)
    perror("Error creating pipe");
  else if (err == 3) {
    perror("command not found");
    exit(127);
  }
  exit(EXIT_FAILURE);
}

void free_tab(char **str) {
  int i;

  i = 0;
  if (!str)
    return;
  while (str[i]) {
    free(str[i]);
    i++;
  }
  free(str);
}

static char *find_path_env(t_data *data) {
  int i;

  i = 0;
  while (data->env->env[i] && ft_strncmp(data->env->env[i], "PATH=", 5) != 0)
    i++;
  if (!data->env->env[i])
    return (NULL);
  return (data->env->env[i] + 5);
}

static char *build_full_path(char *dir, char *cmd) {
  char *temp;
  char *full_path;

  temp = ft_strjoin(dir, "/");
  full_path = ft_strjoin(temp, cmd);
  free(temp);
  return (full_path);
}

char *get_path(char *cmd, t_data *data, char **argv) {
  int i;
  char **path;
  char *good_path;
  char *path_env;

  (void)argv;
  path_env = find_path_env(data);
  if (!path_env)
    return (NULL);
  path = ft_split(path_env, ':');
  i = 0;
  while (path[i]) {
    good_path = build_full_path(path[i], cmd);
    if (access(good_path, X_OK) == 0)
      return (free_tab(path), good_path);
    free(good_path);
    i++;
  }
  free_tab(path);
  return (NULL);
}

void update_shlvl(t_data *data) {
  int i;
  int shlvl;
  char *new_shlvl;
  char *value;

  if (!data || !data->env || !data->env->env)
    return;
  
  i = 0;
  while (data->env->env[i]) {
    if (ft_strncmp(data->env->env[i], "SHLVL=", 6) == 0) {
      value = data->env->env[i] + 6;
      shlvl = ft_atoi(value);
      shlvl++;
      new_shlvl = ft_itoa(shlvl);
      if (new_shlvl) {
        free(data->env->env[i]);
        data->env->env[i] = ft_strjoin("SHLVL=", new_shlvl);
        free(new_shlvl);
      }
      return;
    }
    i++;
  }
}
