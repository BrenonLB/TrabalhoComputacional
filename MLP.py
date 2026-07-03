import numpy as np
import matplotlib.pyplot as plt

def trimf(x, a, b, c):
    return np.maximum(np.minimum((x-a)/(b-a + 1e-9), (c-x)/(c-b + 1e-9)), 0)

x1 = np.linspace(-10, 10, 100)
x2 = np.linspace(-5, 5, 100)
z = np.linspace(-20, 20, 100)

plt.figure(figsize=(10, 6))
plt.subplot(3, 1, 1)
plt.plot(x1, trimf(x1, -10, -10, 0), '#FF6600', label='N')
plt.plot(x1, trimf(x1, -10, 0, 10), '#777777', label='Z')
plt.plot(x1, trimf(x1, 0, 10, 10), '#000000', label='P')
plt.legend()

# (Replique a lógica visual para x2 e z no seu relatório, os domínios estão acima)
plt.show()