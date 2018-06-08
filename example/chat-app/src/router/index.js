import Vue from 'vue'
import Router from 'vue-router'
import Home from '@/layout/Home';
import Login from '@/layout/Login';
import Setting from '@/layout/Setting';
import SessionTest from '@/layout/SessionTest';

Vue.use(Router);

export default new Router({
  routes: [
    {
      path: '/login',
      name : 'login',
      component : Login
    },
    {
      path: '/home',
      name: 'home',
      component: Home
    },
    {
      path : '/setting',
      name : 'setting',
      component : Setting
    },
    {
      path : '/session_test',
      name : 'session_test',
      component : SessionTest
    },


    // default
    {
      path : '/',
      redirect : 'login'
    }
  ]
})
