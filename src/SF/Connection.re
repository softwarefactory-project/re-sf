type connection_type =
  | GIT
  | PAGURE
  | GERRIT
  | GITLAB
  | GITHUB;

type connection = {
  name: string,
  connection_type: connection_type,
  base_url: option(string),
};

let parseConnectionType = json: connection_type => {
  switch (json |> Json.Decode.string) {
    | "git" => GIT
    | "gerrit" => GERRIT
    | "pagure" => PAGURE
    | "github" => GITHUB
    | conn => Util.decodeFail("Unknown connection type: " ++ conn)
  }
}

let parse = json => {
  Json.Decode.{
    connection_type: json |> field("type", parseConnectionType),
    name: json |> field("name", string),
    base_url: json |> optional(field("base-url", string)),
  };
};
