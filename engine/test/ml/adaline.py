def range(start, end, step):
    if step == 0 or step is None:
        step = 1
    if end is None:
        end = start
        start = 0
    if step > 0:
        while start < end:
            yield start
            start += step
    else:
        while start > end:
            yield start
            start += step


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


class Node():
    def __init__(self, parents):
        self.graph = graph
        self.parents = parents
        self.children = []
        self.value = None
        self.jacobi = None

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
                        self.jacobi += child.backward(result) @ child.get_jacobi(self)
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
    def __init__(self, dim, init):
        self.graph = graph
        self.parents = []
        self.children = []
        self.value = None
        self.jacobi = None
        self.graph.add_node(self)
        self.dim = dim
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
        diag = Zeros([row, col])
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


sample_num = 64

train_set = generateSample(sample_num)

# 构造计算图：输入向量，是一个3x1矩阵，不需要初始化，不参与训练
x = Variable([3, 1], False)

# 类别标签，1男，-1女
label = Variable([1, 1], False)

# 权重向量，是一个1x3矩阵，需要初始化，参与训练
w = Variable([1, 3], True)

# 阈值，是一个1x1矩阵，需要初始化，参与训练
b = Variable([1, 1], True)

# ADALINE的预测输出
output = Add([MatrixMultiply([w, x]), b])
predict = Step([output])

# 损失函数
loss = PerceptionLoss([MatrixMultiply([label, output])])

# 学习率
learning_rate = 0.0001

# 训练执行50个epoch
for epoch in range(50):

    # 遍历训练集中的样本
    for i in range(len(train_set)):
        # 取第i个样本的前4列（除最后一列的所有列），构造3x1矩阵对象
        features = train_set[i:i + 1, 0:-1].T.reshape([3, 1])
        # 取第i个样本的最后一列，是该样本的性别标签（1男，-1女），构造1x1矩阵对象
        l = Array([[train_set[i, -1]]])

        # 将特征赋给x节点，将标签赋给label节点
        x.set_value(features)
        label.set_value(l)

        # 在loss节点上执行前向传播，计算损失值
        loss.forward()

        # 在w和b节点上执行反向传播，计算损失值对它们的雅可比矩阵
        w.backward(loss)
        b.backward(loss)
        w.set_value(w.value - w.jacobi.T.reshape(w.get_shape()) * learning_rate)
        b.set_value(b.value - b.jacobi.T.reshape(b.get_shape()) * learning_rate)

        # default_graph对象保存了所有节点，调用clear_jacobi方法清除所有节点的雅可比矩阵
        graph.clear_jacobi()

    accuracy = predict_and_evaluate(train_set, predict, x)

    # 打印当前epoch数和模型在训练集上的正确率
    if epoch % 10 == 0 or epoch == 49:
        print("epoch: ", epoch + 1, " accuracy: ", accuracy)

test_set = generateSample(32)

accuracy = predict_and_evaluate(test_set, predict, x)
print("test accuracy: ", accuracy)
