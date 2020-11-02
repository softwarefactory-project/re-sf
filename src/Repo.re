[@decco]
type t = {
  name: string,
  acl: string,
  description: string,
};

let decode = t_decode;
let encode = t_encode;
