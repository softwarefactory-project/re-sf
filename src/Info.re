[@decco]
type service = {
  name: string,
  path: string,
};

[@decco]
type t = {
  services: list(service),
  version: string,
  header_logo_b64data: string,
  splash_image_b64data: string,
};

let encode = t_encode;
let decode = t_decode;
