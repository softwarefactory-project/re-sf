[@decco]
type t = {
  name: string,
  default_connection: option(string),
  description: option(string),
  url: string,
};

let decode = t_decode;
let encode = t_encode;
