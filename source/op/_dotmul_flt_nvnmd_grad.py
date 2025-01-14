#!/usr/bin/env python3

from tensorflow.python.framework import ops
from deepmd.env import op_module
from deepmd.env import tf 

@ops.RegisterGradient("DotmulFltNvnmd")
def _DotmulFltNvnmdGrad(op, grad):
    x = op.inputs[0]
    w = op.inputs[1]
    # calcualte
    dx = op_module.mul_flt_nvnmd(grad, w)
    dw = op_module.mul_flt_nvnmd(grad, x)
    # add shape for output of matmul_nvnmd
    shx = x.shape.as_list()
    shw = w.shape.as_list()
    shx = [None if (d == -1) else d for d in shx]
    shw = [None if (d == -1) else d for d in shw]
    dx = tf.ensure_shape(dx, shx)
    dw = tf.ensure_shape(dw, shw)
    # print(op.outputs[0], dx, dw)
    return [dx, dw]
