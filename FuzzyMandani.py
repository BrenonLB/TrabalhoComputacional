import numpy as np
import matplotlib.pyplot as plt

np.random.seed(42)

def sigmoid(x):
    return 1 / (1 + np.exp(-x))

def sigmoid_deriv(y):
    return y * (1 - y)

def generate_data(n):
    X = np.random.uniform(-1, 1, (n, 2))
    d = (X[:, 0]**2 + X[:, 1]**2 <= 0.5).astype(int).reshape(-1, 1)
    return X, d

# Conjunto de treinamento e teste
X_train, d_train = generate_data(1000)
X_test, d_test = generate_data(300)

class MLP:
    def __init__(self):
        self.W1 = np.random.uniform(-0.5, 0.5, (2, 10))
        self.b1 = np.random.uniform(-0.5, 0.5, (1, 10))
        self.W2 = np.random.uniform(-0.5, 0.5, (10, 1))
        self.b2 = np.random.uniform(-0.5, 0.5, (1, 1))

    def forward(self, X):
        self.y1 = sigmoid(np.dot(X, self.W1) + self.b1)
        self.y2 = sigmoid(np.dot(self.y1, self.W2) + self.b2)
        return self.y2

    def backward(self, X, d, eta):
        error = d - self.y2

        d2 = error * sigmoid_deriv(self.y2)
        d1 = np.dot(d2, self.W2.T) * sigmoid_deriv(self.y1)

        self.W2 += eta * np.dot(self.y1.T, d2)
        self.b2 += eta * np.sum(d2, axis=0, keepdims=True)

        self.W1 += eta * np.dot(X.T, d1)
        self.b1 += eta * np.sum(d1, axis=0, keepdims=True)

        mse = np.mean(error ** 2)
        return mse


etas = [0.01, 0.1, 0.5]
colors = ['#FF6600', '#000000', '#777777']

plt.figure(figsize=(12,5))

# ===========================
# Item E - MSE vs Épocas
# ===========================

plt.subplot(1,2,1)

models = {}

for eta, color in zip(etas, colors):

    mlp = MLP()

    mse_hist = []

    for epoch in range(1000):

        # Propagação direta
        mlp.forward(X_train)

        # Retropropagação
        mse = mlp.backward(X_train, d_train, eta)

        mse_hist.append(mse)

    models[eta] = mlp

    plt.plot(mse_hist, color=color, label=f'η = {eta}')

plt.title("MSE vs Épocas")
plt.xlabel("Épocas")
plt.ylabel("Erro Quadrático Médio")
plt.grid(True)
plt.legend()

# ===========================
# Item F - Dispersão
# ===========================

plt.subplot(1,2,2)

y_pred = models[0.1].forward(X_test)
y_pred = (y_pred > 0.5).astype(int)

for i in range(len(X_test)):

    if d_test[i] == y_pred[i]:
        marker = 'o'
    else:
        marker = 'x'

    color = '#FF6600' if y_pred[i] == 1 else '#000000'

    plt.scatter(
        X_test[i,0],
        X_test[i,1],
        marker=marker,
        color=color,
        alpha=0.7
    )

plt.title("Dispersão 2D - Teste (η = 0.1)")
plt.xlabel("x1")
plt.ylabel("x2")
plt.grid(True)

plt.tight_layout()
plt.show()