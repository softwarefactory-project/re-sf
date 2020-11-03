[@decco]
type t = {
  name: string,
  [@decco.key "default-connection"]
  default_connection: option(string),
  description: option(string),
  url: string,
};

let decode = t_decode;
let encode = t_encode;
