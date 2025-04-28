import pandas as pd

file_path = 'particle_data_run0.xlsx'  # Replace with your actual file path

# Specify the engine explicitly
df = pd.read_excel(file_path, engine='openpyxl')

print(df.head())
