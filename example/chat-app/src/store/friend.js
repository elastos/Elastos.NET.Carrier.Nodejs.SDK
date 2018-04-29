import _ from 'lodash';
import vue from 'vue';

const default_state = ()=>({
  list : [],
  list_flag : false,
  apply_list : [],
  message : {},

  currentSelect : {}
});

export default {
  state : default_state(),

  mutations : {
    'friend/list/callback'(state, {data}){
      if(!state.list_flag){
        if(!data.end){
          state.list_flag = true;
          state.list = [data.info];
        }

      }
      else{
        if(data.end){
          state.list_flag = false;
        }
        else{
          state.list.push(data.info);
        }
      }
    },
    'friend/list'(state, {data, context}){
      context.commit('add_log', '[friend/list] : ');
    },
    'friend/add'(state, {data, context}){
      context.commit('add_log', '[friend/add] : '+data.address);
    },
    'friend/apply/callback'(state, {data, context}){
      context.commit('add_log', `${data.userId} apply to be your friend with message "${data.msg}"`);

      state.apply_list.push(data);
    },
    'friend/accept'(state, {data, context}){
      context.commit('add_log', '[friend/accept] : ');
    },
    'friend/remove'(state, {data, context}){
      context.commit('add_log', '[friend/remove] : ');
    },
    'friend/remove/callback'(state, {data, context}){
      context.commit('friend.list.remove', {
        userId : data
      });
    },
    'friend/add/callback'(state, {data, context}){
      context.commit('add_log', 'New friend added. userId is '+data.userId);

    },
    'friend/info/callback'(state, {data}){
      const index = _.findIndex(state.list, (l)=>data.userId===l.userId);
      if(index !== -1){
        vue.set(state.list, index, _.extend({}, state.list[index], data));

        if(state.currentSelect.userId === data.userId){
          state.currentSelect = state.list[index];
        }
      }
    },

    'friend/status/callback'(state, {data}){
      const index = _.findIndex(state.list, (l)=>data.userId===l.userId);
      if(index !== -1){
        vue.set(state.list, index, _.extend({}, state.list[index], data));

        // update current select info
        if(state.currentSelect.userId === data.userId){
          state.currentSelect = state.list[index];
        }
      }

    },

    'friend/message'(state){},
    'friend/message/callback'(state, {data, context}){
      context.commit('friend.message.add', {
        key : data.user.userId,
        from : data.user,
        msg : data.msg
      });

      context.commit('add_log', `receive message from ${data.user.userId} : ${data.msg}`);
    },

    'friend.apply_list.remove'(state, item){
      const list = _.clone(state.apply_list);
      _.remove(list, (d)=>d.userId===item.userId);
      state.apply_list = list;
    },
    'friend.list.remove'(state, item){
      const list = _.clone(state.list);
      _.remove(list, (d)=>d.userId===item.userId);
      state.list = list;
    },
    'friend.message.add'(state, item){
      const key = item.key;
      if(!state.message[key]){
        state.message[key] = [];
      }

      // notice here, need to set value to original state
      const list = _.clone(state.message);
      item.date = Date.now();
      list[key].push(item);

      vue.set(state, 'message', list);
    },

    'friend.current.set'(state, item){
      state.currentSelect = item;
    },




    reset(state){
      _.assign(state, default_state());
    }
  },

  getters : {
    getFriendMessageList : (state)=>()=>{
      const key = state.currentSelect.userId;
      return (key && state.message[key])? state.message[key] : [];
    }
  }
};
