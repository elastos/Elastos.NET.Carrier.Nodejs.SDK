
export default {
  getElaId(route){
    // get from location.search
    return route.query.ela || location.search.replace(/^\?/, '');
  }
};
