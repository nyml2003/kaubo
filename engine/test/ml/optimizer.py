class Graph:
    def __init__(self):
        self.nodes = []

    def add_node(self, node):
        self.nodes.append(node)

    def clear_jacobi(self):
        for node in self.nodes:
            node.clear_jacobi()

    def reset_value(self):
        for node in self.nodes:
            node.reset_value()


graph = Graph()


class Node:
    def __init__(self, parents):
        self.graph = graph
        self.parents = parents
        self.children = []
        self.value = None
        self.jacobi = None
        self.trainable = False

        for parent in self.parents:
            parent.children.append(self)

        self.graph.add_node(self)

    def get_parents(self):
        return self.parents

    def get_children(self):
        return self.children

    def forward(self):
        for node in self.parents:
            if node.value is None:
                node.forward()
        self.compute()

    def compute(self):
        pass

    def backward(self, result):
        if self.jacobi is None:
            if self is result:
                self.jacobi = Eye(self.get_dimension())
            else:
                self.jacobi = Zeros([result.get_dimension(), self.get_dimension()])
                for child in self.get_children():
                    if child.value is not None:
                        left = child.backward(result)
                        right = child.get_jacobi(self)
                        self.jacobi += left @ right
        return self.jacobi

    def clear_jacobi(self):
        self.jacobi = None

    def reset_value(self):
        self.value = None
        for child in self.children:
            child.reset_value()

    def get_shape(self):
        return self.value.shape

    def get_dimension(self):
        return self.value.shape[0] * self.value.shape[1]


class Variable(Node):
    def __init__(self, dim, init, trainable):
        self.graph = graph
        self.parents = []
        self.children = []
        self.value = None
        self.jacobi = None
        self.graph.add_node(self)
        self.dim = dim
        self.trainable = trainable
        if init:
            self.value = Normal(0.0, 0.001, self.dim)

    def set_value(self, value):
        self.reset_value()
        self.value = value


class Operator(Node):
    pass


class Add(Operator):
    def compute(self):
        self.value = Zeros(self.parents[0].get_shape())
        for parent in self.parents:
            self.value += parent.value

    def get_jacobi(self, parent):
        return Eye(self.get_dimension())


class MatrixMultiply(Operator):
    def compute(self):
        self.value = self.parents[0].value @ self.parents[1].value

    def get_jacobi(self, parent):
        parent0 = self.parents[0]
        parent1 = self.parents[1]
        lhs_shape = parent0.get_shape()
        m = lhs_shape[0]
        n = lhs_shape[1]
        k = parent1.get_shape()[1]
        if parent is parent0:
            # C 对 A 的雅可比矩阵
            t = parent1.value.T
            jacobi = Zeros([m * k, m * n])
            for i in range(m):
                jacobi[i * k:(i + 1) * k, i * n:(i + 1) * n] = t
            return jacobi
        if parent is parent1:
            jacobi = Zeros([m * k, n * k])
            for i in range(m):
                for j in range(n):
                    jacobi[i * k:(i + 1) * k, j * k:(j + 1) * k] = Eye(k) * parent0.value[i, j]
            return jacobi


class ScalarMultiply(Operator):
    def compute(self):
        self.value = self.parents[0].value * self.parents[1].value

    def get_jacobi(self, parent):
        parent0 = self.parents[0]
        parent1 = self.parents[1]
        if parent is parent0:
            return Diag(parent1.value.ravel())
        if parent is parent1:
            return Eye(parent1.get_dimension()) * self.parents[0].value[0, 0]


class Multiply(Operator):
    def compute(self):
        self.value = self.parents[0].value * self.parents[1].value

    def get_jacobi(self, parent):
        parent0 = self.parents[0]
        parent1 = self.parents[1]
        if parent is parent0:
            return Diag(parent1.value.ravel())
        if parent is parent1:
            return Diag(parent0.value.ravel())


class Step(Operator):
    def compute(self):
        parent0 = self.parents[0]
        shape = parent0.get_shape()
        self.value = Zeros(shape)
        for i in range(shape[0]):
            for j in range(shape[1]):
                if parent0.value[i, j] >= 0.0:
                    self.value[i, j] = 1.0

    def get_jacobi(self, parent):
        return Zeros(self.parents[0].get_shape())


class SoftMax(Operator):
    def compute(self):
        self.value = SoftMax(self.parents[0].value)


class Optimizer(object):
    def __init__(self, graph, target, learning_rate):
        self.graph = graph
        self.target = target
        self.learning_rate = learning_rate
        self.gradient = dict()
        self.steps = 0

    def step(self):
        self.graph.clear_jacobi()
        self.target.forward()
        for node in self.graph.nodes:
            if node.trainable:
                node.backward(self.target)
                gradient = node.jacobi.T.reshape(node.get_shape())
                if node not in self.gradient:
                    self.gradient[node] = gradient
                else:
                    self.gradient[node] += gradient
        self.steps += 1

    def get_gradient(self, node):
        return self.gradient[node] / self.steps

    def update_impl(self):
        pass

    def update(self):
        self.update_impl()
        self.gradient.clear()
        self.steps = 0


class GradientDescent(Optimizer):
    def __init__(self, graph, target, learning_rate):
        Optimizer.__init__(self, graph, target, learning_rate)
        self.learning_rate = learning_rate

    def update_impl(self):
        for node in self.graph.nodes:
            if node.trainable:
                gradient = self.get_gradient(node)
                node.set_value(node.value - gradient * self.learning_rate)


class Momentum(Optimizer):
    def __init__(self, graph, target, learning_rate, momentum):
        Optimizer.__init__(self, graph, target, learning_rate)
        self.learning_rate = learning_rate
        self.momentum = momentum
        self.v = dict()

    def update_impl(self):
        for node in self.graph.nodes:
            if node.trainable:
                gradient = self.get_gradient(node)
                if node not in self.v:
                    self.v[node] = gradient
                else:
                    self.v[node] = self.v[node] * self.momentum - gradient * self.learning_rate
                node.set_value(node.value + self.v[node])


class LossFunction(Node):
    pass


class PerceptionLoss(LossFunction):
    def compute(self):
        parent0 = self.parents[0]
        shape = parent0.get_shape()
        row = shape[0]
        col = shape[1]
        self.value = Zeros(shape)
        for i in range(row):
            for j in range(col):
                if parent0.value[i, j] >= 0.0:
                    self.value[i, j] = 0.0
                else:
                    self.value[i, j] = -parent0.value[i, j]

    def get_jacobi(self, parent):
        parent0 = self.parents[0]
        shape = parent0.get_shape()
        row = shape[0]
        col = shape[1]
        diag = Zeros(shape)
        for i in range(row):
            for j in range(col):
                if parent0.value[i, j] < 0.0:
                    diag[i, j] = -1.0
        return Diag(diag.ravel())


def generateSample(num):
    male_heights = Normal(171.0, 6.0, num)
    male_weights = Normal(70.0, 10.0, num)
    male_bfrs = Normal(16.0, 2.0, num)
    female_weights = Normal(57.0, 8.0, num)
    female_heights = Normal(158.0, 5.0, num)
    female_bfrs = Normal(22.0, 2.0, num)

    male_labels = [1.0] * num
    female_labels = [-1.0] * num
    train_set = Concatenate(
        [Array([male_heights + female_heights]),
         Array([male_weights + female_weights]),
         Array([male_bfrs + female_bfrs]),
         Array([male_labels + female_labels])
         ]).T
    Shuffle(train_set)
    return train_set


sample_num = 512
train_set = generateSample(sample_num)
x = Variable([3, 1], False, False)
label = Variable([1, 1], False, False)
w = Variable([1, 3], True, True)
b = Variable([1, 1], True, True)
output = Add([MatrixMultiply([w, x]), b])
predict = Step([output])
loss = PerceptionLoss([MatrixMultiply([label, output])])
learning_rate = 0.01
optimizer = Momentum(graph, loss, learning_rate, 0.9)

batch_size = 32
current_batch_size = 0


def predict_and_evaluate(sample_set, predict, input):
    pred = []
    for i in range(len(sample_set)):
        features = sample_set[i:i + 1, 0:-1].T.reshape([3, 1])
        input.set_value(features)
        predict.forward()
        pred.append(predict.value[0, 0])

    for i in range(len(pred)):
        pred[i] *= 2.0
        pred[i] -= 1.0
    total = 0
    predicted = sample_set.T.ravel()[-len(sample_set)::1]
    for i in range(len(pred)):
        if pred[i] == predicted[i]:
            total += 1
    accuracy = total / len(sample_set)
    return accuracy


for epoch in range(50):
    for i in range(len(train_set)):
        features = train_set[i:i + 1, 0:-1].T.reshape([3, 1])
        l = Array(train_set[i, -1]).reshape([1, 1])
        x.set_value(features)
        label.set_value(l)
        optimizer.step()
        current_batch_size += 1
        if current_batch_size == batch_size:
            optimizer.update()
            current_batch_size = 0

    accuracy = predict_and_evaluate(train_set, predict, x, batch_size)

    # 打印当前epoch数和模型在训练集上的正确率
    print("epoch: ", epoch + 1, " accuracy: ", accuracy)

test_set = generateSample(32)

accuracy = predict_and_evaluate(test_set, predict, x, batch_size)
print("test accuracy: ", accuracy)
