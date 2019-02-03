typedef cpu_state;

cpu_state* init_cpu(memory);

int run_command(cpu_state*, op_code);
