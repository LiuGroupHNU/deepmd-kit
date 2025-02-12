#!/usr/bin/env python3

from tensorflow.python.framework import ops
from deepmd.env import op_module
from deepmd.env import tf 

@ops.RegisterGradient("MapFltNvnmd")
def _MapFltNvnmdGrad(op, grad):
    x = op.inputs[0]
    table = op.inputs[1]
    table_grad = op.inputs[2]
    table_info = op.inputs[3]
    # shape
    shx = x.shape.as_list()
    shw = table.shape.as_list()
    shx = [None if (d == -1) else d for d in shx]
    shw = [None if (d == -1) else d for d in shw]
    N = shx[0]
    D = shx[1]
    M = shw[1] // 4
    #
    dydx = op_module.map_flt_nvnmd(x, table_grad, tf.zeros_like(table_grad), table_info)
    dydx = tf.ensure_shape(dydx, [N, D, M])
    # calculate
    dx = op_module.dotmul_flt_nvnmd(dydx, grad)
    dx = tf.reshape(dx, [-1, D])
    # dx = dydx*grad
    # dx = tf.reshape(dx, [-1, M])
    # dx = tf.reduce_sum(dx, axis=1)
    # dx = tf.reshape(dx, [-1, D])
    # other grad
    d_table = None
    d_table_grad = None
    d_table_info = None
    # print(op.outputs[0], dx)
    return [dx, d_table, d_table_grad, d_table_info]

