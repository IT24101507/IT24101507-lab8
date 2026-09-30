import pandas as pd
import matplotlib.pyplot as plt


df = pd.read_csv('results.csv')


avg_df = df.groupby(['program', 'np'])['time'].mean().reset_index()


def calc_speedup(group):
    t1 = group.loc[group['np'] == 1, 'time'].values[0]
    group['speedup'] = t1 / group['time']
    return group

avg_df = avg_df.groupby('program', group_keys=False).apply(calc_speedup)

# 4. Plot Execution Time and Speedup
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))

for prog in avg_df['program'].unique():
    subset = avg_df[avg_df['program'] == prog]
    ax1.plot(subset['np'], subset['time'], marker='o', label=prog)
    ax2.plot(subset['np'], subset['speedup'], marker='o', label=prog)

# Ideal linear speedup reference line
nps = avg_df['np'].unique()
ax2.plot(nps, nps, 'k--', alpha=0.5, label='Ideal Linear')

# Graph details
ax1.set_title('Execution Time vs. MPI Processes')
ax1.set_xlabel('Number of Processes (np)')
ax1.set_ylabel('Mean Execution Time (s)')
ax1.grid(True)
ax1.legend()

ax2.set_title('Speedup vs. MPI Processes')
ax2.set_xlabel('Number of Processes (np)')
ax2.set_ylabel('Speedup (T1 / Tnp)')
ax2.grid(True)
ax2.legend()

plt.tight_layout()
plt.savefig('benchmark_plots.png', dpi=300)
print("Saved plots to benchmark/benchmark_plots.png")