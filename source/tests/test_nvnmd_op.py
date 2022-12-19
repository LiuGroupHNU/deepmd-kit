import os
import sys
import numpy as np
import unittest

import deepmd.op
from deepmd.env import tf
from deepmd.env import op_module
from deepmd.env import GLOBAL_TF_FLOAT_PRECISION
from deepmd.env import GLOBAL_NP_FLOAT_PRECISION
from deepmd.env import GLOBAL_ENER_FLOAT_PRECISION


class TestOpAddFltNvnmd(tf.test.TestCase):
    def setUp(self):
        config = tf.ConfigProto()
        if int(os.environ.get("DP_AUTO_PARALLELIZATION", 0)):
            config.graph_options.rewrite_options.custom_optimizers.add().name = "dpparallel"
        self.sess = self.test_session(config=config).__enter__()
    
    def test_op(self):
        # graph
        t_x = tf.placeholder(tf.float64, [None, 1], 't_x')
        t_y = op_module.add_flt_nvnmd(t_x, t_x)
        # feed_dic
        x = np.reshape(np.arange(0, 8**2) / 3.0, [-1, 1])
        feed_dict = {
            t_x : x
        }
        # get value and test
        self.sess.run(tf.global_variables_initializer())
        y_pred = self.sess.run(t_y, feed_dict=feed_dict)
        y_test = np.array([
             0.,  0.66666651,  1.33333302,  2.,   2.66666603,  3.33333206,
             4.,  4.66666412,  5.33333206,  6.,   6.66666412,  7.33333206,
             8.,  8.66666412,  9.33332825, 10.,  10.66666412, 11.33332825,
            12., 12.66666412, 13.33332825, 14.,  14.66666412, 15.33332825,
            16., 16.66665649, 17.33332825, 18.,  18.66665649, 19.33332825,
            20., 20.66665649, 21.33332825, 22.,  22.66665649, 23.33332825,
            24., 24.66665649, 25.33332825, 26.,  26.66665649, 27.33332825,
            28., 28.66665649, 29.33332825, 30.,  30.66665649, 31.33332825,
            32., 32.66665649, 33.33331299, 34.,  34.66665649, 35.33331299,
            36., 36.66665649, 37.33331299, 38.,  38.66665649, 39.33331299,
            40., 40.66665649, 41.33331299, 42.
        ])
        y_pred = np.reshape(y_pred, [-1])
        y_test = np.reshape(y_test, [-1])
        np.testing.assert_almost_equal(y_test, y_pred, 5)
        tf.reset_default_graph()


class TestOpCopyFltNvnmd(tf.test.TestCase):
    def setUp(self):
        config = tf.ConfigProto()
        if int(os.environ.get("DP_AUTO_PARALLELIZATION", 0)):
            config.graph_options.rewrite_options.custom_optimizers.add().name = "dpparallel"
        self.sess = self.test_session(config=config).__enter__()
    
    def test_op(self):
        # graph
        t_x = tf.placeholder(tf.float64, [None, 1], 't_x')
        t_y, t_y2 = op_module.copy_flt_nvnmd(t_x)
        # feed_dic
        x = np.reshape(np.arange(0, 8) / 3.0, [-1, 1])
        feed_dict = {
            t_x : x
        }
        # get value and test
        self.sess.run(tf.global_variables_initializer())
        y_pred, y_pred2= self.sess.run([t_y, t_y2], feed_dict=feed_dict)
        y_test = np.array([
            0., 0.33333325, 0.66666651, 1.,
            1.33333302, 1.66666603, 2., 2.33333206
        ])
        y_pred = np.reshape(y_pred, [-1])
        y_pred2 = np.reshape(y_pred2, [-1])
        y_test = np.reshape(y_test, [-1])
        np.testing.assert_almost_equal(y_test, y_pred, 5)
        np.testing.assert_almost_equal(y_test, y_pred2, 5)
        tf.reset_default_graph()


class TestOpDotmulFltNvnmd(tf.test.TestCase):
    def setUp(self):
        config = tf.ConfigProto()
        if int(os.environ.get("DP_AUTO_PARALLELIZATION", 0)):
            config.graph_options.rewrite_options.custom_optimizers.add().name = "dpparallel"
        self.sess = self.test_session(config=config).__enter__()
    
    def test_op(self):
        # graph
        t_x = tf.placeholder(tf.float64, [None, 4], 't_x')
        t_y = op_module.dotmul_flt_nvnmd(t_x, t_x)
        # feed_dic
        x = np.reshape(np.arange(0, 8) / 3.0, [-1, 4])
        feed_dict = {
            t_x : x
        }
        # get value and test
        self.sess.run(tf.global_variables_initializer())
        y_pred = self.sess.run(t_y, feed_dict=feed_dict)
        y_test = np.array([
            1.55555, 13.99998
        ])
        y_pred = np.reshape(y_pred, [-1])
        y_test = np.reshape(y_test, [-1])
        np.testing.assert_almost_equal(y_test, y_pred, 5)
        tf.reset_default_graph()


class TestOpFltNvnmd(tf.test.TestCase):
    def setUp(self):
        config = tf.ConfigProto()
        if int(os.environ.get("DP_AUTO_PARALLELIZATION", 0)):
            config.graph_options.rewrite_options.custom_optimizers.add().name = "dpparallel"
        self.sess = self.test_session(config=config).__enter__()
    
    def test_op(self):
        # graph
        t_x = tf.placeholder(tf.float64, [None, 4], 't_x')
        t_y = op_module.flt_nvnmd(t_x)
        # feed_dic
        x = np.reshape(np.arange(0, 8) / 3.0, [-1, 4])
        feed_dict = {
            t_x : x
        }
        # get value and test
        self.sess.run(tf.global_variables_initializer())
        y_pred = self.sess.run(t_y, feed_dict=feed_dict)
        y_test = np.array([
            0., 0.33333, 0.66667, 1., 1.33333, 1.66667, 2., 2.33333
        ])
        y_pred = np.reshape(y_pred, [-1])
        y_test = np.reshape(y_test, [-1])
        np.testing.assert_almost_equal(y_test, y_pred, 5)
        tf.reset_default_graph()


class TestOpMapFltNvnmd(tf.test.TestCase):
    def setUp(self):
        config = tf.ConfigProto()
        if int(os.environ.get("DP_AUTO_PARALLELIZATION", 0)):
            config.graph_options.rewrite_options.custom_optimizers.add().name = "dpparallel"
        self.sess = self.test_session(config=config).__enter__()

    def test_op(self):
        mapt = np.load('nvnmd/map.npy', allow_pickle=True)[0]
        table = GLOBAL_NP_FLOAT_PRECISION(np.concatenate([mapt['s'][0], mapt['h'][0]], axis=1))
        table_grad = GLOBAL_NP_FLOAT_PRECISION(np.concatenate([mapt['s_grad'][0], mapt['h_grad'][0]], axis=1))
        table_info =  GLOBAL_NP_FLOAT_PRECISION(np.reshape(mapt['cfg_u2s'], [-1]))
        # graph
        t_x = tf.placeholder(tf.float64, [None, 1], 't_x')
        t_table = tf.placeholder(tf.float64, [None, None], 't_table')
        t_table_grad = tf.placeholder(tf.float64, [None, None], 't_table_grad')
        t_table_info = tf.placeholder(tf.float64, [None], 't_table_info')
        t_y = op_module.map_flt_nvnmd(t_x, t_table, t_table_grad, t_table_info)
        # feed_dic
        x = np.reshape(np.arange(0, 8**2), [-1, 1])
        feed_dict = {
            t_x : x,
            t_table : table,
            t_table_grad : table_grad * 0.0,
            t_table_info : np.reshape(np.array(table_info), [-1])
        }
        # get value and test
        self.sess.run(tf.global_variables_initializer())
        y_pred = self.sess.run(t_y, feed_dict=feed_dict)
        y_test = np.array([
            -4.02932405e-01,  0.00000000e+00,  1.27062531e+01,  2.10604095e+01,
             8.86666107e+00,  1.05302048e+01,  7.16565704e+00,  7.02013779e+00,
             6.15166092e+00,  5.26510239e+00,  5.45392990e+00,  4.20795822e+00,
             4.91504288e+00,  3.48788261e+00,  4.46474457e+00,  2.95572662e+00,
             4.06997681e+00,  2.54059982e+00,  3.71370125e+00,  2.20451164e+00,
             3.38652611e+00,  1.92516804e+00,  3.08297348e+00,  1.68853760e+00,
             2.79967117e+00,  1.48526478e+00,  2.53443527e+00,  1.30881405e+00,
             2.28576660e+00,  1.15443516e+00,  2.05257607e+00,  1.01856136e+00,
             1.83401871e+00,  8.98437500e-01,  1.62939548e+00,  7.91882038e-01,
             1.43810177e+00,  6.97134972e-01,  1.25958920e+00,  6.12747669e-01,
             1.09334564e+00,  5.37512302e-01,  9.38878059e-01,  4.70406055e-01,
             7.95710087e-01,  4.10553455e-01,  6.63372517e-01,  3.57197762e-01,
             5.41402817e-01,  3.09679031e-01,  4.29343462e-01,  2.67416716e-01,
             3.26739788e-01,  2.29896545e-01,  2.33141899e-01,  1.96660519e-01,
             1.48102522e-01,  1.67298198e-01,  7.11788535e-02,  1.41440034e-01,
             1.93022378e-03,  1.18751287e-01, -6.00755513e-02,  9.89289284e-02,
            -1.15272462e-01,  8.16950202e-02, -1.64083123e-01,  6.67971969e-02,
            -2.06929684e-01,  5.40025234e-02, -2.44227648e-01,  4.30970192e-02,
            -2.76385307e-01,  3.38837802e-02, -3.03807735e-01,  2.61801332e-02,
            -3.26895952e-01,  1.98162049e-02, -3.46041203e-01,  1.46353990e-02,
            -3.61629009e-01,  1.04917213e-02, -3.74043703e-01,  7.24812597e-03,
            -3.83658171e-01,  4.77796420e-03, -3.90845537e-01,  2.96119228e-03,
            -3.95965099e-01,  1.68743543e-03, -3.99380922e-01,  8.50534532e-04,
            -4.01440382e-01,  3.53393843e-04, -4.02492762e-01,  1.03041355e-04,
            -4.02877808e-01,  1.26575978e-05, -4.02932405e-01,  0.00000000e+00,
            -4.02932405e-01,  0.00000000e+00, -4.02932405e-01,  0.00000000e+00,
            -4.02932405e-01,  0.00000000e+00, -4.02932405e-01,  0.00000000e+00,
            -4.02932405e-01,  0.00000000e+00, -4.02932405e-01,  0.00000000e+00,
            -4.02932405e-01,  0.00000000e+00, -4.02932405e-01,  0.00000000e+00,
            -4.02932405e-01,  0.00000000e+00, -4.02932405e-01,  0.00000000e+00,
            -4.02932405e-01,  0.00000000e+00, -4.02932405e-01,  0.00000000e+00,
            -4.02932405e-01,  0.00000000e+00, -4.02932405e-01,  0.00000000e+00
        ])
        y_pred = np.reshape(y_pred, [-1])
        y_test = np.reshape(y_test, [-1])
        np.testing.assert_almost_equal(y_test, y_pred, 5)
        tf.reset_default_graph()


class TestOpMatmulFitnetNvnmd(tf.test.TestCase):
    def setUp(self):
        config = tf.ConfigProto()
        if int(os.environ.get("DP_AUTO_PARALLELIZATION", 0)):
            config.graph_options.rewrite_options.custom_optimizers.add().name = "dpparallel"
        self.sess = self.test_session(config=config).__enter__()
    
    def test_op(self):
        # graph
        t_x = tf.placeholder(tf.float64, [None, 4], 't_x')
        t_w = tf.placeholder(tf.float64, [4, 4], 't_w')
        t_y = op_module.matmul_fitnet_nvnmd(t_x, t_w, 23, 19, 1)
        # feed_dic
        x = np.reshape(np.arange(0, 16) / 3.0, [-1, 4])
        feed_dict = {
            t_x : x,
            t_w : x
        }
        # get value and test
        self.sess.run(tf.global_variables_initializer())
        y_pred = self.sess.run(t_y, feed_dict=feed_dict)
        y_test = np.array([
            6.22222,  6.88888,  7.55555,  8.22222,
            16.88887, 19.33331, 21.77776, 24.22221,
            27.55553, 31.77774, 35.99997, 40.2222 ,
            38.22219, 44.22217, 50.22218, 56.22219
        ])
        y_pred = np.reshape(y_pred, [-1])
        y_test = np.reshape(y_test, [-1])
        np.testing.assert_almost_equal(y_test, y_pred, 5)
        tf.reset_default_graph()


class TestOpMatmulFltNvnmd(tf.test.TestCase):
    def setUp(self):
        config = tf.ConfigProto()
        if int(os.environ.get("DP_AUTO_PARALLELIZATION", 0)):
            config.graph_options.rewrite_options.custom_optimizers.add().name = "dpparallel"
        self.sess = self.test_session(config=config).__enter__()
    
    def test_op(self):
        # graph
        t_x = tf.placeholder(tf.float64, [None, 4], 't_x')
        t_w = tf.placeholder(tf.float64, [4, 4], 't_w')
        t_y = op_module.matmul_flt_nvnmd(t_x, t_w, 0, 0)
        # feed_dic
        x = np.reshape(np.arange(0, 16) / 3.0, [-1, 4])
        feed_dict = {
            t_x : x,
            t_w : x
        }
        # get value and test
        self.sess.run(tf.global_variables_initializer())
        y_pred = self.sess.run(t_y, feed_dict=feed_dict)
        y_test = np.array([
            6.22222,  6.88888,  7.55555,  8.22221, 
            16.88887, 19.33331, 21.77776, 24.2222 , 
            27.55553, 31.77774, 35.99997, 40.2222 ,
            38.22217, 44.22217, 50.22217, 56.22217
        ])
        y_pred = np.reshape(y_pred, [-1])
        y_test = np.reshape(y_test, [-1])
        np.testing.assert_almost_equal(y_test, y_pred, 5)
        tf.reset_default_graph()


class TestOpMatmulFltNvnmd(tf.test.TestCase):
    def setUp(self):
        config = tf.ConfigProto()
        if int(os.environ.get("DP_AUTO_PARALLELIZATION", 0)):
            config.graph_options.rewrite_options.custom_optimizers.add().name = "dpparallel"
        self.sess = self.test_session(config=config).__enter__()
    
    def test_op(self):
        # graph
        t_x = tf.placeholder(tf.float64, [None, 4], 't_x')
        t_w = tf.placeholder(tf.float64, [4, 4], 't_w')
        t_y = op_module.matmul_flt_nvnmd(t_x, t_w, 0, 0)
        # feed_dic
        x = np.reshape(np.arange(0, 16) / 3.0, [-1, 4])
        feed_dict = {
            t_x : x,
            t_w : x
        }
        # get value and test
        self.sess.run(tf.global_variables_initializer())
        y_pred = self.sess.run(t_y, feed_dict=feed_dict)
        y_test = np.array([
            6.22222,  6.88888,  7.55555,  8.22221, 
            16.88887, 19.33331, 21.77776, 24.2222 , 
            27.55553, 31.77774, 35.99997, 40.2222 ,
            38.22217, 44.22217, 50.22217, 56.22217
        ])
        y_pred = np.reshape(y_pred, [-1])
        y_test = np.reshape(y_test, [-1])
        np.testing.assert_almost_equal(y_test, y_pred, 5)
        tf.reset_default_graph()


class TestOpMatmulFlt2fixNvnmd(tf.test.TestCase):
    def setUp(self):
        config = tf.ConfigProto()
        if int(os.environ.get("DP_AUTO_PARALLELIZATION", 0)):
            config.graph_options.rewrite_options.custom_optimizers.add().name = "dpparallel"
        self.sess = self.test_session(config=config).__enter__()
    
    def test_op(self):
        # graph
        t_x = tf.placeholder(tf.float64, [None, 4], 't_x')
        t_w = tf.placeholder(tf.float64, [4, 4], 't_w')
        t_y = op_module.matmul_flt2fix_nvnmd(t_x, t_w, 23)
        # feed_dic
        x = np.reshape(np.arange(0, 16) / 3.0, [-1, 4])
        feed_dict = {
            t_x : x,
            t_w : x
        }
        # get value and test
        self.sess.run(tf.global_variables_initializer())
        y_pred = self.sess.run(t_y, feed_dict=feed_dict)
        y_test = np.array([
            6.22222,  6.88888,  7.55555,  8.22221, 
            16.88887, 19.33331, 21.77776, 24.2222 , 
            27.55554, 31.77776, 35.99997, 40.2222 ,
            38.2222 , 44.2222 , 50.22217, 56.2222 
        ])
        y_pred = np.reshape(y_pred, [-1])
        y_test = np.reshape(y_test, [-1])
        np.testing.assert_almost_equal(y_test, y_pred, 5)
        tf.reset_default_graph()

class TestOpMulFltNvnmd(tf.test.TestCase):
    def setUp(self):
        config = tf.ConfigProto()
        if int(os.environ.get("DP_AUTO_PARALLELIZATION", 0)):
            config.graph_options.rewrite_options.custom_optimizers.add().name = "dpparallel"
        self.sess = self.test_session(config=config).__enter__()
    
    def test_op(self):
        # graph
        t_x = tf.placeholder(tf.float64, [None, 4], 't_x')
        t_w = tf.placeholder(tf.float64, [4, 4], 't_w')
        t_y = op_module.mul_flt_nvnmd(t_x, t_w)
        # feed_dic
        x = np.reshape(np.arange(0, 16) / 3.0, [-1, 4])
        feed_dict = {
            t_x : x,
            t_w : x
        }
        # get value and test
        self.sess.run(tf.global_variables_initializer())
        y_pred = self.sess.run(t_y, feed_dict=feed_dict)
        y_test = np.array([
            0.     ,  0.11111,  0.44444,  1.     ,
            1.77778,  2.77777, 4.     ,  5.44444,
            7.11111,  9.     , 11.1111 , 13.44444,
            16.     , 18.77776, 21.77774, 25.
        ])
        y_pred = np.reshape(y_pred, [-1])
        y_test = np.reshape(y_test, [-1])
        np.testing.assert_almost_equal(y_test, y_pred, 5)
        tf.reset_default_graph()


class TestOpQuantizeNvnmd(tf.test.TestCase):
    def setUp(self):
        config = tf.ConfigProto()
        if int(os.environ.get("DP_AUTO_PARALLELIZATION", 0)):
            config.graph_options.rewrite_options.custom_optimizers.add().name = "dpparallel"
        self.sess = self.test_session(config=config).__enter__()
    
    def test_op(self):
        # graph
        t_x = tf.placeholder(tf.float64, [None, 4], 't_x')
        t_y = op_module.quantize_nvnmd(t_x, 0, 23, 23, -1)
        # feed_dic
        x = np.reshape(np.arange(0, 8) / 3.0, [-1, 4])
        feed_dict = {
            t_x : x
        }
        # get value and test
        self.sess.run(tf.global_variables_initializer())
        y_pred = self.sess.run(t_y, feed_dict=feed_dict)
        y_test = np.array([
            0., 0.33333, 0.66667, 1., 1.33333, 1.66667, 2., 2.33333
        ])
        y_pred = np.reshape(y_pred, [-1])
        y_test = np.reshape(y_test, [-1])
        np.testing.assert_almost_equal(y_test, y_pred, 5)
        tf.reset_default_graph()


class TestOpTanh4FltNvnmd(tf.test.TestCase):
    def setUp(self):
        config = tf.ConfigProto()
        if int(os.environ.get("DP_AUTO_PARALLELIZATION", 0)):
            config.graph_options.rewrite_options.custom_optimizers.add().name = "dpparallel"
        self.sess = self.test_session(config=config).__enter__()
    
    def test_op(self):
        # graph
        t_x = tf.placeholder(tf.float64, [None, 4], 't_x')
        t_y = op_module.tanh4_flt_nvnmd(t_x)
        # feed_dic
        x = np.reshape(np.arange(0, 8) / 3.0, [-1, 4])
        feed_dict = {
            t_x : x
        }
        # get value and test
        self.sess.run(tf.global_variables_initializer())
        y_pred = self.sess.run(t_y, feed_dict=feed_dict)
        y_test = np.array([
            0., 0.32485, 0.60494, 0.8125, 0.93827, 0.99151, 1., 1.     
        ])
        y_pred = np.reshape(y_pred, [-1])
        y_test = np.reshape(y_test, [-1])
        np.testing.assert_almost_equal(y_test, y_pred, 5)
        tf.reset_default_graph()


if __name__ == '__main__':
    unittest.main()
