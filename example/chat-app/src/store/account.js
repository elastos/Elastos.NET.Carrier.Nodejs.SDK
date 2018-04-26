export default {
  namespaced : true,
  state : {
    username : null
  },

  mutations : {
    username(state, name){
      state.username = name;
    }
  }
}
