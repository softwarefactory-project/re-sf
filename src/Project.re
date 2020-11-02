[@decco]
type t = {
  name: string,
  description: string,
  tenant: option(string),
  connection: option(string),
  website: option(string),
  options: option(list(string)),
  documentation: option(string),
  [@decco.key "issue-tracker-url"]
  issue_tracker_url: option(string),
  [@decco.key "review-dashboard"]
  review_dashboard: option(string),
  [@decco.key "mailing-lists"]
  mailing_lists: option(list(string)),
  contacts: option(list(string)),
  [@decco.key "source-repositories"]
  sourceRepositories:
    [@decco.codec SourceRepository.codec] SourceRepository.unions,
};

let decode = t_decode;
let encode = t_encode;

/* optionEq compares an optional value `x` with a value `y` using the (==) operator */
let optionEq = (x: option('t), y: 't) => Belt.Option.eq(x, Some(y), (==));

let filterProjectsByTenant = (tenant: string, projects: list(t)) => {
  projects->Belt.List.keep(project => project.tenant->optionEq(tenant));
};
