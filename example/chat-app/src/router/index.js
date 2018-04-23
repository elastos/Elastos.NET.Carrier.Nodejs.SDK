import Vue from 'vue'
import Router from 'vue-router'
import Home from '@/layout/Home';
import Login from '@/layout/Login';

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



    //default
    {
      path : '/',
      redirect : 'home'
    }
  ]
})
